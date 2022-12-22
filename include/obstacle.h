#pragma once

class Obstacle{
    public:
        virtual ~Obstacle() = 0;
        virtual bool destroyable() = 0;
        virtual void set_destroy_stat(bool) = 0;
        virtual bool is_blocking_movement() = 0;
        virtual bool set_block(bool) = 0;
        virtual float speed_coof() = 0;
        virtual void set_speed_coof(float) = 0;
};

class IceObstacle: public Obstacle{

};

class WaterObstacle: public Obstacle{

};

class WallObstacle: public Obstacle{
    public:
    virtual ~WallObstacle() = 0;
    virtual int get_hp() = 0;
    virtual void set_hp(int);
    virtual void damage(int);
};

class IronWall:public WallObstacle{

};

class StoneWall:public WallObstacle{

};