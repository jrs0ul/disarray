#include <disarray/gui/EditBox.h>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL_scancode.h>
    #else
        #include <SDL2/SDL_scancode.h>
    #endif
#else
    #ifndef __ANDROID__
        #include <SDL2/SDL_scancode.h>
    #endif
#endif

#include <disarray/gui/Text.h>
#include <disarray/ActionKeys.h>



void EditBox::init(unsigned int dx, unsigned int dy, const char* dtitl, unsigned char dmaxl)
{
    setpos(dx, dy);
    strcpy(title,dtitl);
    maxlength=dmaxl;
    entered=false;
    canceled=false;
    strcpy(text,"");
#ifndef __ANDROID__
    pressedKey = SDL_SCANCODE_RETURN;
#endif
}
//------------------------------------
void EditBox::getInput(const char* eventText, unsigned keydown,
                       const unsigned char* keys, const unsigned char* oldKeys)
{

    if (keys[ACTION_BACK] && !oldKeys[ACTION_BACK])
    {
        canceled = true;
        return;
    }

#ifndef __ANDROID__
    if (pressedKey != keydown)
    {
        if ( keydown == SDL_SCANCODE_RETURN)
        {
            entered = true;
        }
        else if ( keydown == SDL_SCANCODE_ESCAPE )
        {
            canceled = true;
        }
        else if (keydown == SDL_SCANCODE_BACKSPACE)
        {
            if (strlen(text))
            {
                text[strlen(text) - 1] = 0;
            }
        }
        else if (strlen(text) < 20 && keydown != 0)
        {
            strcat(text, eventText);
        }
    }

    pressedKey = keydown;

#endif
}
//----------------------------
void EditBox::setText(const char* newText)
{
    strncpy(text, newText, 20);
}

//------------------------------------
void EditBox::draw(SpriteBatcher& pics, unsigned font)
{

    WriteShadedText(getX() + 2, getY() + 1, pics, font, title);

    if ((rand() % 100) % 5 == 0)
    {
        char tmp[80];
        sprintf(tmp,"%s_",text);   
        WriteShadedText(getX() + 5, getY() + 16, pics, font,tmp);
    }
    else
    {
        WriteShadedText(getX() + 5, getY() + 16, pics, font, text);
    }

}
//----------------------------------------------
void EditBox::reset()
{
    strcpy(text,"");
    entered = false;
    canceled = false;
#ifndef __ANDROID__
    pressedKey = SDL_SCANCODE_RETURN;
#endif
}

