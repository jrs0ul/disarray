#ifndef	_SELECTMENIU_H_
#define _SELECTMENIU_H_

#include <cstring>
#include "BasicUiControl.h"
#include "../SpriteBatcher.h"

struct TouchData;

struct MenuOption
{
    char text[30];
    int pic;
    int id;
};


//--------------------------------------

class Vector3D;

class SelectMenu : public BasicControl
{
    public:
        char title[30];
        std::vector<MenuOption> options;
        unsigned pressedkey;
        unsigned int width;
        unsigned int height;
        unsigned char defstate;
        unsigned char state;

        bool selected;
        bool canceled;

        SelectMenu(){selected = false; canceled = false;}

        void init(unsigned int dx,
                  unsigned int dy,
                  const char* dtitle,
                  std::vector<MenuOption>& dsel,
                  unsigned char dstate,
                  unsigned int dheight = 0);
        // resets the selection
        void reset();
        //changes the state depending on input keys
        void getInput(const unsigned char* keys, const unsigned char* oldKeys, TouchData& touches);
        void draw(SpriteBatcher& pics, unsigned rod,  unsigned font, unsigned icons = 0);
    private:
        bool isHittingOptions(Vector3D& v);

};

#endif
