#include "recog.h"
/*
 * 读取储存的模型文件，
 * 在detect的过程中，从其进程里读取脸部图片，进行predict
 */
Recog::Recog()
{

}

void Recog::run()
{
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->load("./model.yml");


}
