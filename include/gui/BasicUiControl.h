#ifndef _BASICUICONTROL_
#define _BASICUICONTROL_


//--------------------------------------

    class ActiveControl{
        //is control active
        bool activated;
    public:
        ActiveControl() : activated(false){}
        void activate(){if (!activated) activated = true;}
        bool active(){return activated;}
        void deactivate(){if (activated) activated = false;}
    };
//----------------------------------------

    class BasicControl: public ActiveControl {
        int x;
        int y;
    public:
        void setpos(int nx, int ny){x = nx; y = ny;}
        int getX(){ return x;}
        int getY(){ return y;}
    };




#endif //_BASICUICONTROL_
