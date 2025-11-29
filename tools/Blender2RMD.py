import bpy
import Blender
from array import array


#======================================================
def _exportSubsetAtributes(out, ob):
	
	#let's put object matrix to file

	m = ob.getMatrix()
			
	print m

	_matrix = array('f')
	for v in m :
		for f in v :
			_matrix.append(f)

	_matrix.tofile(out)
	
	#---
				
	stuff= array('f')
			
	eul = ob.getEuler("worldspace")
	#print eul
	for rot in eul:
		stuff.append(rot)
		
	loc = ob.getLocation("worldspace")
	for pos in loc:
		stuff.append(pos)
		
	siz = ob.getSize("worldspace")
	for sc in siz:
		stuff.append(sc)
				
	stuff.tofile(out)			

	#print _matrix

#=====================================================

def export(filename) :


	out = file(filename,'wb+')

	print "------Let's GO!---------"
	print "export file: " + filename

	totalFrames = Blender.Get("endframe") - Blender.Get("staframe")+1

	framec = array('i')
	framec.append(totalFrames)
	framec.tofile(out)
	framec.pop

	print "Total Frames: "+ str(totalFrames)

	
	
	sce = bpy.data.scenes.active
	obs = sce.objects #all objects

	objc = array('i')

	objectcount = 0
	for ob in obs :
		if (ob.getType() == 'Mesh'):
			objectcount+=1

	objc.append(objectcount)
	objc.tofile(out)
	objc.pop

	print 'Total meshes:'+str(objectcount)

	for ob in obs :
		
		if (ob.getType() == 'Mesh'): #is object mesh ?
		
			print ob.getName()
			
			_exportSubsetAtributes(out, ob)			

			#get the mesh data			
			#for all frames

			for i in range(Blender.Get("staframe"),Blender.Get("endframe")+1):	
				

			

				mesh = bpy.data.meshes.new()
				
				#set the right frame
				Blender.Set("curframe",i)
				print "Frame :"+str(i)+" of "+str(totalFrames)

				mesh.getFromObject(ob.getName());#,1)
				
				
				
						
			
				vertex = array('f')
				normals = array('f')
				faces = array('i')
				vertc = array('i')
				uvs = array('f')


				for vert in mesh.verts :
					vertex.append(vert.co.x)
					vertex.append(vert.co.z)
					vertex.append(vert.co.y*-1)
					
					normals.append(vert.no.x*-1)
					normals.append(vert.no.z*-1)
					normals.append(vert.no.y*-1)
		
		
				vertc.append(len(mesh.verts))
				print 'vertexu buvo: ' + str(len(mesh.verts))
				vertc.tofile(out)	
				vertex.tofile(out)
				normals.tofile(out)
	
	
				if (i == Blender.Get("staframe") ):
					
					vertc.pop()
					facecount=0
	
					for face in mesh.faces :
		
						# is triangle
						if (len(face.v)==3):
							faces.append(face.v[2].index) 
							faces.append(face.v[1].index) 
							faces.append(face.v[0].index)
						
						
							facecount+=1
			
							if (mesh.faceUV==1):
								uvs.append(face.uv[2][0])
								uvs.append(face.uv[2][1])
								uvs.append(face.uv[1][0])
								uvs.append(face.uv[1][1])
								uvs.append(face.uv[0][0])
								uvs.append(face.uv[0][1])
							
	
						#not triangle ?
						else :
					
							
							faces.append(face.v[2].index)
							
							if (mesh.faceUV==1):
								uvs.append(face.uv[2][0])
								uvs.append(face.uv[2][1])
							
							faces.append(face.v[1].index)
							
							if (mesh.faceUV==1):
								uvs.append(face.uv[1][0])
								uvs.append(face.uv[1][1])
							
							faces.append(face.v[0].index)
							
							if (mesh.faceUV==1):
								uvs.append(face.uv[0][0])
								uvs.append(face.uv[0][1])
							
									
							faces.append(face.v[3].index)
							if (mesh.faceUV==1):
								uvs.append(face.uv[3][0])
								uvs.append(face.uv[3][1])
							
							faces.append(face.v[2].index)
							if (mesh.faceUV==1):
								uvs.append(face.uv[2][0])
								uvs.append(face.uv[2][1])	
							
							faces.append(face.v[0].index)
							if (mesh.faceUV==1):	
								uvs.append(face.uv[0][0])
								uvs.append(face.uv[0][1])
			
							facecount+=2
						
					#end for
	
	
					print 'feisu: '+str(facecount)
		
					vertc.append(facecount)
					vertc.tofile(out)
					faces.tofile(out)
					vertc.pop()
					if (mesh.faceUV==1):
						vertc.append(1)
					else:
						vertc.append(0)
					vertc.tofile(out)
					if (mesh.faceUV==1):
						uvs.tofile(out)
				
					mesh.verts = None
					
				#end of if (i==0)	
	
	
	out.close()
	
Blender.Window.FileSelector(export,"Export")
