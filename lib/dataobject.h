#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include"common.h"
#include"frame.h"

class Group {
private:
    std::queue <Chary> input_filename;
    int crop_w,crop_h,pan_x,pan_y;
    float fps_v,brightness_v,saturation_v;
    effects effect_v;
public:
    Group();

    void addFileName(const Chary& c);

    Chary getFilename() throw(const Empty&);

	void popOne() throw(const Empty&);

    int number();

    void fps(float f);
    void brightness(float b);
    void saturation(float s);
    void crop(float w,float h);
    void pan(int x,int y);
    void effect(effects e);

    effects effect() const;
    float fps() const;
    float brightness() const;
    float saturation() const;
    int cropWidth() const ;
    int cropHeight() const ;
    int panX() const ;
    int panY() const ;

    ~Group();
};

class DataObject {
    Chary output_filename, audio_filename;
    int output_width,output_height;

    // int quality;
public:
    std::queue <Group> group;

    DataObject(const Chary& o_f,int o_w,int o_h);

    int number() const;

	int width() const;

	int height() const;

    void addGroup(const Group& g);

    Group getGroup() throw(const Empty&);

    Chary outputFilename()const;

    Chary audioFilename() const;

    void audioFilename(const Chary& f);

    ~DataObject();
};



inline int Group::number() {
    return input_filename.size();
}

inline void Group::fps(float f) {
    fps_v =f;
}
inline void Group::brightness(float b) {
    brightness_v = b;
}
inline void Group::saturation(float s) {
    saturation_v = s;
}
inline void Group::crop(float w,float h) {
    crop_w = w;
    crop_h = h;
}
inline void Group::pan(int x,int y) {
    pan_x = x;
    pan_y = y;
}

inline void Group::effect(effects e) {
    effect_v = e;
}

inline effects Group::effect() const {
    return effect_v;
}

inline float Group::fps() const {
    return fps_v;
}
inline float Group::brightness() const {
    return brightness_v;
}
inline float Group::saturation() const {
    return saturation_v;
}
inline int Group::cropWidth() const {
    return crop_w;
}
inline int Group::cropHeight() const {
    return crop_h;
}
inline int Group::panX() const {
    return pan_x;
}
inline int Group::panY() const {
    return pan_y;
}


inline  int DataObject::number() const {
    return group.size();
}


inline Chary DataObject::outputFilename() const {
    return output_filename;
}

inline  Chary DataObject::audioFilename() const {
    return audio_filename;
}

inline  void DataObject::audioFilename(const Chary& f) {
    audio_filename = f;
}

inline int DataObject::width() const{
	return output_width;
}

inline int DataObject::height() const{
	return output_height;
}


#endif // DATAOBJECT_H
