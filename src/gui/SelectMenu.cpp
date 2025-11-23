#include <gui/SelectMenu.h>
#include <gui/Text.h>
#include <TouchData.h>


void SelectMenu::init(unsigned int dx,
                      unsigned int dy,
                      const char* dtitle,
                      std::vector<MenuOption>& dsel,
                      unsigned char dstate,
                      unsigned int dheight)
{
    setpos(dx, dy);
    strcpy(title, dtitle);

    options = dsel;

    if ((strlen(title) == 0) || (strlen(options[0].text) > strlen(title)))
    {
        width = (int)strlen(options[0].text) * 11 + 70;
    }
    else
    {
        width = (int)strlen(title) * 11 + 70;
    }


    if (dheight == 0)
    {
        height = options.size() * 20 + 50;
    }
    else
    {
        height = dheight;
    }

    defstate = dstate;
    state = defstate;
    selected = false; //haven't selected anything yet
    canceled = false;
    deactivate();
    pressedkey = 0;
}
//--------------------------------------------
void SelectMenu::reset()
{
    state = defstate;
    selected = false;
    canceled = false;
}


bool SelectMenu::isHittingOptions(Vector3D& v)
{
    return  v.x > (int)getX() && v.x < (int)(getX() + width) &&
                    v.y >= (int)getY() + 26 && v.y < (int)(getY() + height);

}

//--------------------------------------------
void SelectMenu::getInput(const unsigned char* keys, const unsigned char* oldKeys, TouchData& touches)
{

    if (!touches.allfingersup)
    {
        if (!touches.down.empty())
        {
            if (isHittingOptions(touches.down[0]))
            {
                state = (touches.down[0].y - getY() - 26) / 20;
                state = (state >= options.size()) ? options.size() - 1 : state;
            }
        }

        if (!touches.move.empty())
        {
            if (isHittingOptions(touches.move[0]))
            {
                state = (touches.move[0].y - getY() - 26) / 20;
                state = (state >= options.size()) ? options.size() - 1 : state;
            }
        }
    }


    if (!touches.up.empty())
    {
        if (isHittingOptions(touches.up[0]))
        {
            state = (touches.up[0].y - getY() - 26) / 20;
            state = (state >= options.size()) ? options.size() - 1 : state;
            selected = true;
            return;
        }
    }



    if (keys[4] && !oldKeys[4]) 
    {
        if (options.size())
        {
            selected = true;      //enter
        }
    }

    if (keys[5] && !oldKeys[5]) 
    {
        canceled = true;      //esc
    }

    if (options.size())
    {

        if (keys[0] && !oldKeys[0])
        {
            if (state > 0)
            {//up
                state--;
            }
            else
            {
                state = options.size() - 1;
            }
        }
        else if (keys[1] && !oldKeys[1])
        {

            if (state < options.size() - 1)
            {//down
                state++;
            }
            else
            {
                state = 0;
            }
        }

    }

}
//--------------------------------------------
void SelectMenu::draw(SpriteBatcher& pics, unsigned rod,  unsigned font, unsigned icons)
{
    const unsigned OPTION_HEIGHT = 20;

    unsigned newcount = 0;
    unsigned start = 0;
    int half = (((height - 28) / OPTION_HEIGHT) / 2);
    unsigned tmpheight = height;

    if (options.size() * OPTION_HEIGHT + 28 > tmpheight)
    {
        if (((tmpheight - 28) / OPTION_HEIGHT) + state / half > options.size())
        {
            newcount = options.size();
        }
        else
        {
            newcount = ((height - 28) / OPTION_HEIGHT) + state / half;
        }

        start = state / half;
    }
    else
    {
        newcount = options.size();
    }

    if ((icons) && (((newcount - start) * pics.getInfo(icons)->theight) + 28 > (unsigned)tmpheight))
    {
        height = (newcount-start) * pics.getInfo(icons)->theight + 40;
    }

    WriteText(getX() + 12, getY() + 2, pics, font, title, 1.0f, 1.0f, COLOR(0,0,0, 1.f), COLOR(0,0,0,1.f)); 
    WriteText(getX() + 10, getY() + 4, pics, font, title); 


    for (unsigned i = start; i < newcount; i++)
    {
        if (icons)
        {
            pics.draw(icons, getX() + 16, getY() + 28 + ((i - start) * pics.getInfo(icons)->theight), options[i].pic);
            WriteText(getX() + 20 + pics.getInfo(icons)->theight,
                    getY() + 28 + ((i-start) * pics.getInfo(icons)->theight),
                    pics, font, options[i].text, 1.0f, 1.2f, COLOR(0,0,0, 1.f));

            WriteText(getX() + 20 + pics.getInfo(icons)->theight,
                    getY() + 28 + ((i-start) * pics.getInfo(icons)->theight),
                    pics, font, options[i].text);

        }
        else
        {
            WriteText(getX() + 34, getY() + 26 + ((i - start) * OPTION_HEIGHT),
                      pics, font, options[i].text, 1.0f, 1.0f, COLOR(0,0,0,1.f), COLOR(0,0,0,1.f));
            WriteText(getX() + 32, getY() + 28 + ((i - start) * OPTION_HEIGHT), pics, font, options[i].text);

        }
    }


    if (start>0)
    {
        pics.draw(rod, getX() + width-18, getY() + 3, 1);
    }

    if (newcount < options.size())
    {
        pics.draw(rod, getX() + width-18, getY() + height-19,2);
    }

    int space = 20;

    if (icons)
    {
        space = pics.getInfo(icons)->theight;
    }

    if ((rand() % 100) % 10 == 0)
    {
        pics.draw(rod, getX() + 3, getY() + 28+((state-start)*space));
    }
    else
    {
        pics.draw(rod, getX() + 5, getY() + 28+((state-start)*space));
    }

}

