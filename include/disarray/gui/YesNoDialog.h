#ifndef YESNODIALOG_H
#define YESNODIALOG_H

#include "Button.h"

class YesNoDialog: public BasicControl{
    int width;
    int height;
    Button yes;
    Button no;
    
public:
    YesNoDialog(){}
    YesNoDialog(int _x, int _y, int w, int h){
        set(_x, _y, w, h);
    }

    void set(int _x, int _y, int w, int h);
    void draw(PicsContainer& pics, unsigned font, unsigned decorations,
              unsigned buttons, unsigned buttonframes, float decorationSize = 64.0f);
    bool isPointerOnYes(int px, int py);
    bool isPointerOnNo(int px, int py);
    
};



#endif //YESNODIALOG_H

