import bpy
import Blender
from array import array
from Blender import Mathutils
#--------------------------------------------

def ObjectCount(objs, type):
    objectcount = 0
    for ob in objs :
        if (ob.getType() == type):
            objectcount+=1
            
    return objectcount

#---------------------------------------------
#writes bone geometry to file
def ExportGeometry(mesh, matrix, out, swapyz):
    
    vertex = array('f')
    normals = array('f')
    for vert in mesh.verts :
        vt = vert.co * matrix
        if swapyz == True:
            vertex.append(vt.x)
            vertex.append(vt.z)
            vertex.append(vt.y*-1)

            normals.append(vert.no.x * -1)
            normals.append(vert.no.z * -1)
            normals.append(vert.no.y)
        else:
            vertex.append(vt.x)
            vertex.append(vt.y)
            vertex.append(vt.z)

            normals.append(vert.no.x)
            normals.append(vert.no.y)
            normals.append(vert.no.z)

    vertc = array('i')
    vertc.append(len(mesh.verts))
    vertc.tofile(out)
    vertex.tofile(out)
    normals.tofile(out)

    faces = array('i')
    uvs = array('f')
    
    facecount=0
    for face in mesh.faces :
        # is triangle
        if (len(face.v)==3):

            if (swapyz == True):
                faces.append(face.v[2].index) 
                faces.append(face.v[1].index) 
                faces.append(face.v[0].index)
            else:
                faces.append(face.v[0].index) 
                faces.append(face.v[2].index) 
                faces.append(face.v[1].index)


            facecount+=1

            if (mesh.hasFaceUV()==1):
                if (swapyz == True):
                    uvs.append(face.uv[2][0])
                    uvs.append(face.uv[2][1])
                    uvs.append(face.uv[1][0])
                    uvs.append(face.uv[1][1])
                    uvs.append(face.uv[0][0])
                    uvs.append(face.uv[0][1])
                else:
                    uvs.append(face.uv[0][0])
                    uvs.append(face.uv[0][1])
                    uvs.append(face.uv[2][0])
                    uvs.append(face.uv[2][1])
                    uvs.append(face.uv[1][0])
                    uvs.append(face.uv[1][1])
                            

        #not triangle ?
        else :
            faces.append(face.v[2].index)
            if (mesh.hasFaceUV()==1):
                uvs.append(face.uv[2][0])
                uvs.append(face.uv[2][1])

            faces.append(face.v[1].index)
            if (mesh.hasFaceUV()==1):
                uvs.append(face.uv[1][0])
                uvs.append(face.uv[1][1])

            faces.append(face.v[0].index)
            if (mesh.hasFaceUV()==1):
                uvs.append(face.uv[0][0])
                uvs.append(face.uv[0][1])

            if (swapyz == True):
                faces.append(face.v[3].index)
                if (mesh.hasFaceUV()==1):
                    uvs.append(face.uv[3][0])
                    uvs.append(face.uv[3][1])

                faces.append(face.v[2].index)
                if (mesh.hasFaceUV()==1):
                    uvs.append(face.uv[2][0])
                    uvs.append(face.uv[2][1])

                faces.append(face.v[0].index)
                if (mesh.hasFaceUV()==1):
                    uvs.append(face.uv[0][0])
                    uvs.append(face.uv[0][1])
            else:
                faces.append(face.v[0].index)
                if (mesh.hasFaceUV()==1):
                    uvs.append(face.uv[0][0])
                    uvs.append(face.uv[0][1])

                faces.append(face.v[2].index)
                if (mesh.hasFaceUV()==1):
                    uvs.append(face.uv[2][0])
                    uvs.append(face.uv[2][1])

                faces.append(face.v[3].index)
                if (mesh.hasFaceUV()==1):
                    uvs.append(face.uv[3][0])
                    uvs.append(face.uv[3][1])


            facecount+=2
    #end for
    facec = array('i')
    facec.append(facecount)
    facec.tofile(out)
    print facecount
    faces.tofile(out)
    uvsc = array('i')
    if (mesh.hasFaceUV()==1):
        uvsc.append(1)
    else:
        uvsc.append(0)
    uvsc.tofile(out)
    if (mesh.hasFaceUV()==1):
        uvs.tofile(out)
    
#-----------------------------------------------
def WriteInfluences(BoneList, mesh, out):

    for vert in mesh.verts:
        influences = mesh.getVertexInfluences(vert.index)

        sum_influences = sum([influence[1] for influence in influences])
        
        boneIndices = array('i')
        boneWeights = array('f')

        boneIndices.append(len(influences))

        for influence in influences:
            boneIndices.append(BoneList.index(influence[0]))
            boneWeights.append(influence[1]/sum_influences)

        boneIndices.tofile(out)
        boneWeights.tofile(out)

#----------------------------------------
#writes bone state in frame i
def exportFrame(_armature, i, bone, out, swap):
    #if (i == 30):
    #    print "===Frame :"+ str(i)+"========"
    Blender.Set("curframe",i)
    pose = _armature.getPose()
    armpos = _armature.mat

    identity = Mathutils.Matrix([1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1])
    rotation = Mathutils.RotationMatrix(-90, 4, 'x')
    swapyz = rotation * identity


    if (swap == True):
        initial = Mathutils.Matrix(bone.matrix['ARMATURESPACE']) * armpos * swapyz
        initial.invert()
        posematrix = Mathutils.Matrix(pose.bones[bone.name].poseMatrix) * armpos * swapyz
    else:
        initial = Mathutils.Matrix(bone.matrix['ARMATURESPACE']) * armpos
        initial.invert()
        posematrix = Mathutils.Matrix(pose.bones[bone.name].poseMatrix) * armpos

    
    finalmatrix = initial * posematrix

    #if (i==30):
    #    print _armature.mat
    
    floats = array('f')
    #if (i == 30):
    #    print finalmatrix
    for a in range(4):
        for b in range(4):
            floats.append(finalmatrix[a][b])

    floats.tofile(out)
    
    #export only pose matrix
    """poser = array('f')
    for a in range(4):
        for b in range(4):
            poser.append(posematrix[a][b])
    
    poser.tofile(out)"""


#--------------------------------------------
def export(filename):

    out = file(filename,'wb+')
    scene = bpy.data.scenes.active
    Objects = scene.objects

    magic = array('c')
    magic.fromstring('RM2')
    magic.tofile(out)

    
    ints = array('i')
    
    ints.append(ObjectCount(Objects, 'Mesh'))
    ints.tofile(out)
    ints.pop

    swapyz = True
    
    for ob in Objects:
        if (ob.getType()== 'Mesh'):
            mesh = ob.getData()

            m = Mathutils.Matrix(ob.getMatrix())

            ExportGeometry(mesh, m, out, swapyz)

            if (ob.parent != None):
                parent = ob.parent

                if (parent.getType() == 'Armature'):
                    ArmatureData = parent.getData()
                    Bones = ArmatureData.bones
                    BoneData = Bones.values()

                    bonec = array('i')
                    bonec.append(len(BoneData))
                    bonec.tofile(out)

                    BoneList = []
                    for bone in BoneData:
                        BoneList.append(bone.name)

                    WriteInfluences(BoneList, mesh, out)

                    totalFrames = Blender.Get("endframe") -\
                                  Blender.Get("staframe") + 1

                    framec = array('i')
                    framec.append(totalFrames)
                    framec.tofile(out)
                    framec.pop

                    for bone in BoneData:
                        for i in range(Blender.Get("staframe"),Blender.Get("endframe")+1):
                            exportFrame(parent, i, bone, out, swapyz)
                        #for each frame

                #if armature
            else:
                bonec = array('i')
                bonec.append(0)
                bonec.append(0)
                bonec.tofile(out)

    out.close()


#----------------------------------------------

Blender.Window.FileSelector(export,"Export")
