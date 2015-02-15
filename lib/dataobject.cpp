#include "common.h"
#include "dataobject.h"
#include"frame.h"

Group::Group():crop_w(0),crop_h(0),pan_x(0),pan_y(0),fps_v(0.0),brightness_v(0.0),saturation_v(1.0),effect_v(NONE) {}

void Group::addFileName(const Chary& c) {
    input_filename.push(c);
}

Chary Group::getFilename() throw(const Empty&) {
    if(input_filename.empty())
        throw Empty("Couldn't get filename.","Filename");

    Chary temp = input_filename.front();
    input_filename.pop();
    return temp;

}


void Group::popOne() throw(const Empty&) {
    if(input_filename.empty())
        throw Empty("Couldn't get filename.","Filename");
    input_filename.pop();
}



Group::~Group() {}


DataObject::DataObject(const Chary& o_f,int o_w,int o_h):output_filename(o_f),output_width(o_w),output_height(o_h) {

}

void DataObject::addGroup(const Group& g) {
    group.push(g);
}

Group DataObject::getGroup() throw(const Empty&) {
    if(group.empty())
        throw Empty("Couldn't get group.","Group");

    Group temp = group.front();
    group.pop();
    return temp;
}

DataObject::~DataObject() {}
