#pragma once

#include "bullet.h"

class Tank
{
public:
    virtual ~Tank() = default;
    virtual void level_up() = 0;
    virtual const int &get_curr_level() = 0;
    virtual int set_curr_level() = 0;
    virtual int get_hp() = 0;
    virtual int set_hp(int) = 0;
    virtual bool is_destroyed();
    virtual void recover();
    virtual Bullet shoot() = 0; 
    virtual int get_bullet_freq() = 0;
};