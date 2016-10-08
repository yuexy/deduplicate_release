#include "KUSOU.hpp"
#include "imgretrieval.h"
#include "BOW.h"
#include"include_sense_image_search.h"
#include <time.h>
#include<string>
#include"mat2md5.h"
#include<omp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
using namespace std;
using namespace cv;
#define KUSOU_EXPORTS

static err_msg last_err;


float normConfid1(float confiOri, float m_gvThreshold);

SF_SDK_API sf_handle_t sf_create_image_searcher(){
    try{
        imgRetrieval* irP=new imgRetrieval();
        irP->m_path="";
        irP->m_gvThreshold=0.044;
        irP->m_featureType="SIFT";

        return (void*) irP;
    }
    catch(...){
        last_err.err_type=SF_E_FAIL;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "fail to create image searcher", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_create_image_searcher", sizeof(last_err.call_func));
    }
}


// @brief 销毁已初始化的图像搜索句柄
// @param filter_handle 已初始化的图像搜索句柄
SF_SDK_API void sf_destroy_image_searcher( sf_handle_t search_handle){
 try{
        imgRetrieval* irP = (imgRetrieval*) search_handle;
        irP->~imgRetrieval();
        delete search_handle;
        return;
    }
  catch(...){
        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "fail to release image searcher", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_destroy_image_searcher", sizeof(last_err.call_func));
    }
}


// @brief 进行图像搜索检测
// @param filter_handle 用于搜索的句柄
// @param filter_handle 被搜索的图片库句柄
// @param image_data 用于待检测的输入图像
// @param image_width,image_height, image_stride 图像尺寸信息
// @param result_array 图像搜索结果数组
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_search_image(
    sf_handle_t  search_handle,
    sf_handle_t  db_handle,
    unsigned char *image_data,
    sf_pixel_format pixel_format,
    int image_width,
    int image_height,
    int image_stride,
    sf_search_result & result_array
)
{
    imgRetrieval* irP;
    try {
      irP=(imgRetrieval*) db_handle;
    }
    catch(...){

        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid database handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_search_image", sizeof(last_err.call_func));

        result_array.value=SF_RLT_NOTCAL;
        return SF_E_HANDLE;
    }



    if(pixel_format!=SF_PIX_FMT_BGR888||image_width<=0||image_height<=0) {

        last_err.err_type=SF_E_INVALIDARG;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid image format", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_search_image", sizeof(last_err.call_func));

        result_array.value = SF_RLT_NOTCAL;
        return SF_E_INVALIDARG;
    }

    cv:Mat img(image_height, image_width, CV_8UC3, image_data);
    img.step=image_stride;

    vector<uint> imgIds; vector<float>imgVals;

    searchImg_gv(*irP, img, imgIds, imgVals);
    if(imgVals.size()<=0) {
        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=1;
        strncpy(last_err.err_msg, "invalid database handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_search_image", sizeof(last_err.call_func));
        result_array.img_index=-1;
        result_array.similarity=0;
        result_array.value=-1;
        return SF_RLT_NOTCAL;
    }
    if(imgVals.at(0)>irP->m_gvThreshold)
    {
        result_array.value=SF_RLT_PASS;
        result_array.img_index =imgIds.at(0);
        result_array.similarity= normConfid1(imgVals.at(0), irP->m_gvThreshold);
    }
    else if(imgVals.at(0)<irP->m_gvThreshold){
        result_array.value=SF_RLT_NOPASS;
        result_array.img_index =imgIds.at(0);
        result_array.similarity=normConfid1(imgVals.at(0), irP->m_gvThreshold);
    }
    else{
        result_array.value=SF_RLT_PENDING;
    }
    return SF_OK;
}

// @brief 销毁图像搜索结果
// @param result_array 图像搜索结果数组
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_destroy_search_result( sf_search_result* result_array){
    try{
        delete result_array;
        result_array=NULL;
        return SF_OK;
    }
    catch(...){

        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid search handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_destroy_search_result", sizeof(last_err.call_func));
    }
}

// 图像库管理函数组

// @brief 创建数据库句柄
// @return 成功返回图像数据库句柄，失败返回NULL
SF_SDK_API sf_handle_t sf_create_image_db(){
    imgRetrieval* irP=new imgRetrieval();

    //valid time check
     struct tm *newtime;
     time_t timep;
     time(&timep);
     newtime=localtime(&timep);
     bool outDated=false;
     if(newtime->tm_year+1900>2017){
         outDated=true; cout<<"Invalid date"<<endl;
     }
     else if((newtime->tm_year+1900)==2017&& (newtime->tm_mon+1)>4){
         outDated=true; cout<<"Invalid date"<<endl;
     }
     if(outDated) {
         last_err.err_type=SF_WARNING;
         last_err.err_level=2;
         strncpy(last_err.err_msg, "Invalid date", sizeof(last_err.err_msg));
         strncpy(last_err.call_func , "sf_create_image_db", sizeof(last_err.call_func));
         return NULL;
     }
/*
    irP->m_path="";
    try{
        irP->loadHKM("word.st");
    }
     catch(...){
         last_err.err_type=SF_E_DBNOTBOUND;
         last_err.err_level=2;
         strncpy(last_err.err_msg, "FAIL to find 'word.st' at current path", sizeof(last_err.err_msg));
         strncpy(last_err.call_func , "sf_create_image_db", sizeof(last_err.call_func));
         return NULL;
        }
    */
    irP->m_gvThreshold=0.044;
    irP->m_featureType="SIFT";
    return (void*) irP;
}

// @brief 销毁已初始化的数据库句柄
// @param db_handle 已初始化的人脸数据库句柄
SF_SDK_API void sf_destroy_image_db( sf_handle_t db_handle){
    //delete db_handle;
    imgRetrieval* irP = (imgRetrieval*) db_handle;
    irP->~imgRetrieval();
    delete db_handle;
}

// @brief 图像数据库增加数据
// @param db_handle 已初始化的图像数据库句柄
// @param image_data 用于待添加的的图像
// @param image_width,image_height, image_stride 图像尺寸
// @param idx 加入后的图像数据库索引值
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_add_image(
    sf_handle_t db_handle,
    unsigned char *image_data,
    sf_pixel_format pixel_format,
    int image_width,
    int image_height,
    int image_stride,
    int *idx
){ 
    imgRetrieval* irP;
    try {
      irP=(imgRetrieval*) db_handle;
    }
    catch(...){
        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid database handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_add_image", sizeof(last_err.call_func));

        return SF_E_HANDLE;
    }


    if(pixel_format!=SF_PIX_FMT_BGR888||image_width<=0||image_height<=0) {

        last_err.err_type=SF_E_INVALIDARG;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "Invalid image", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_add_image", sizeof(last_err.call_func));
        return SF_E_INVALIDARG;
    }
    cv:Mat img(image_height, image_width, CV_8UC3, image_data);
    img.step=image_stride;

    //extract features
    vector<cv::KeyPoint> keyPoints;
    cv::Mat descriptors;
    string ftrType="SIFT";
    imgRetrieval::extractImgFeature(img,keyPoints,descriptors, ftrType);
    int num=keyPoints.size();
    if(num==0) return SF_E_INVALIDARG;
    //quantization of the feature
    Data<float> points;
    imgRetrieval::cvMat2Data(descriptors,points,false);
    uint* classId=new uint[num];
    irP->m_phkm->getLeafIds(points, classId);
    pair<uint*, uint> pv;
    pv.first = classId;
    pv.second = num;

    pair<attribute*, uint> pv_orientation;

    attribute* orientation = new attribute[num];
    for(int k=0;k<num;k++)
    {
        //orientation[k].orientation=keyPoints.at(k).angle/360.0*2*CV_PI;
        //orientation[k].scale=keyPoints.at(k).octave;
        orientation[k].x=keyPoints.at(k).pt.x;
        orientation[k].y=keyPoints.at(k).pt.y;
    }
    pv_orientation.first = orientation;
    pv_orientation.second = num;

    //update the inverted files
     //update word count and docs list
    ivDoc doc;
    doc.ntokens=num;
    int docIdx= irP->m_ivfile.docs.size();
    irP->m_ivfile.ndocs+=1;
    irP->m_ivfile.docs.push_back(doc);
    *idx = docIdx;

     //loop update
    int nbranches=irP->m_phkm->hkms.at(0).opt.nbranches;
    int nlevels=irP->m_phkm->hkms.at(0).opt.nlevels;
    uint nwords = pow((double)nbranches, nlevels);
    irP->m_ivfile.words.resize(nwords); irP->m_ivfile.nwords=nwords;
    for (uint t=0; t<num; t++){
        if(pv.first[t]==0||pv.first[t]>irP->m_ivfile.nwords) continue;
        uint wl=(uint)(pv.first[t]-1);
        //get word
        ivWord* w = & irP->m_ivfile.words[wl];
        w->wf++;
        //make a new word doc
        ivWordDoc newdoc;
        newdoc.doc =docIdx;
        ivWordDocIt wdit=lower_bound(w->docs.begin(),w->docs.end(), newdoc);
        if (wdit == w->docs.end() || newdoc<(*wdit))
        {
          //wdit = w->docs.insert(wdit, newdoc);
          w->ndocs++;
        }
        wdit = w->docs.insert(wdit, newdoc);

        //increment word count for this word's doc
        //wdit->count++;
        wdit->attri=(attribute)pv_orientation.first[t];
    }

    // update weights and words of docs;
    //irP->m_ivfile.computeStats(ivFile::WEIGHT_TFIDF, ivFile::NORM_L1);
    delete []orientation;
    delete []classId;
  //  sf_destroy_image_searcher(irP);
    return SF_OK;
}

// @brief 图像数据库删除数据
// @param db_handle 已初始化的图像数据库句柄
// @param idx 待删除的图像数据库索引值(与加入时一致)
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_delete_image( sf_handle_t db_handle, int index){

    //imgRetrieval* irP = (imgRetrieval*) (db_handle);
    imgRetrieval* irP;
    try {
      irP=(imgRetrieval*) db_handle;
    }
    catch(...){

        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid database handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_delete_image", sizeof(last_err.call_func));
        return SF_E_HANDLE;
    }

    irP->m_ivfile.ndocs--;
    //update the words that are contain in given image
    for (int t=0; t<irP->m_ivfile.docs.at(index).nwords; t++){
        uint wl=irP->m_ivfile.docs.at(index).words.at(t);
        if(wl==0||wl>irP->m_ivfile.nwords) continue;
        ivWord* w= &irP->m_ivfile.words.at(wl);
        //loop over to decrease the word count;
        for (int i=0; i<w->ndocs;i++){
            if (w->docs.at(i).doc==index){
                w->ndocs--;
                w->wf-=(w->docs.at(i).count);
                w->docs.erase(w->docs.begin()+i);
               // break;
            }
        }
    }

    return SF_OK;
}

// @brief 将图像数据库信息保存为数据库文件
// @param db_handle 已初始化的图像数据库句柄
// @param db_path 图像数据库保存文件的路径
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_save_image_db( sf_handle_t db_handle,const char *db_path){
    //imgRetrieval* irP =(imgRetrieval*) db_handle;
    imgRetrieval* irP;
    try {
      irP=(imgRetrieval*) db_handle;
    }
    catch(...){
        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid database handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_save_image_db", sizeof(last_err.call_func));
        return SF_E_HANDLE;
    }

    irP->m_ivfile.save(db_path);
    return SF_OK;
}

// @brief 加载图像数据库文件
// @param db_handle 已初始化的图像数据库句柄
// @param db_path 图像数据库保存文件的路径
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_load_image_db( sf_handle_t db_handle, const char *db_path){

    imgRetrieval* irP=(imgRetrieval*) db_handle;
    try{
        irP->loadIVF(db_path);
    }
    catch(...){
        cout<<"fail to laod the inverted file";

        last_err.err_type=SF_E_INVALIDARG;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "fail to load database", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_load_image_db", sizeof(last_err.call_func));

        return SF_E_INVALIDARG;
    }
    return SF_OK;
}

//＠brief 更新特征的权重
//＠param 已初始化的图像数据库句柄
//＠return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_update_weights(sf_handle_t db_handle){
    try{
        imgRetrieval* irP=(imgRetrieval*) db_handle;
        irP->m_ivfile.computeStats(irP->m_wt, irP->m_norm);
    }
    catch(...){
        cout<<"Invalid handle"<<endl;

        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "invalid handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_update_weights", sizeof(last_err.call_func));

        return SF_E_HANDLE;
    }
    return SF_OK;

}

float normConfid1(float confiOri, float m_gvThreshold){
   // Normalize the postive confidence
   if(confiOri> m_gvThreshold){
       if(confiOri>0.06)
           return 1;
       else {
            return (-m_gvThreshold+confiOri)/(-m_gvThreshold+0.06);
       }
   }
   // Normalize the negative confidence
   else if(confiOri<=m_gvThreshold){
       if(confiOri<=0)

           return 1;
       else{
           return ((m_gvThreshold-confiOri)/m_gvThreshold);
       }
   }
}

//@brief 寻找词典中重复的图片
//@param ".txt文件,保存invertedFile中文件的路径"
//@param "初始化的图像数据库句柄"
//@return "vector<pair<string md51, string md52>>"
//@return "成功返回SF_OK,否则返回错误类型"
SF_SDK_API sf_result_t  sf_find_nearly_duplicate(sf_handle_t db_handle, string imgPathList,
                                         vector<pair<string , string > > & dupPair){
    // load db path set
    imgRetrieval* irP=(imgRetrieval*) db_handle;
    irP->m_trainFileName=imgPathList;
    irP->readTrainData();

    if(irP->m_trainNames.size()!=irP->m_ivfile.ndocs){
        cout<<"number of docs in the inverted file and train name list mismatch";
        return SF_E_INVALIDARG;
    }



    #pragma omp parallel for
    for (int i=0; i<irP->m_trainNames.size();i++ ){
        pair<string, string> tmpPair;
        vector<uint> imgIds;
        vector<float> imgVals;       
        Mat img= cv::imread(irP->m_trainNames.at(i));

        cout<<i<<"/"<<irP->m_trainNames.size()<<endl;
        if(img.cols==0) {
            cout<<"fail to open "<<irP->m_trainNames.at(i)<<endl;
            continue;
        }

        searchImg_gv(*irP, img, imgIds, imgVals);

        //find Pair

        for(int ii=0; ii<imgIds.size();ii++){
            if(imgVals.at(ii)>irP->m_gvThreshold){
                if(imgIds.at(ii)==i) continue;

                tmpPair.first=str2md5((char*)img.data, (int)img.step[0] * img.rows);
                Mat img2=cv::imread(irP->m_trainNames.at(imgIds.at(ii)));

                if(img2.cols==0) {
                    cout<<"fail to open "<<irP->m_trainNames.at(imgIds.at(ii))<<endl;
                    continue;
                }

                tmpPair.second = str2md5((char*)img2.data, (int) img2.step[0]*img2.rows);
                dupPair.push_back(tmpPair);
                cout<<tmpPair.first<<"  "<<tmpPair.second<<" "<<i<<" "<<imgIds.at(ii)<<endl;

            }
            else{
                break;
            }
        }
    }

}

//@brief 创建聚类中心的handle
//成功返回handle，失败返回NULL
SF_SDK_API sf_handle_t sf_create_word_handle(const char *word_path){
     Hkms<float>* m_phkm= new(Hkms<float>);
     try{
        m_phkm->loadFromBinaryFile(word_path);
     }
     catch(...){
         last_err.err_type=SF_E_DBNOTBOUND;
         last_err.err_level=2;
         strncpy(last_err.err_msg, "FAIL to find 'word.st' at current path", sizeof(last_err.err_msg));
         strncpy(last_err.call_func , "sf_create_word_handle", sizeof(last_err.call_func));
         return NULL;
        }
     return (void*) m_phkm;
}

// @brief 销毁已初始化的聚类中心(word)的handle
// @param filter_handle 已初始化的聚类中心(word)的handle
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_destroy_word_handle(sf_handle_t word_handle){
  try{
    Hkms<float>* m_phkm= (Hkms<float>*) word_handle;
    m_phkm->clear();
    delete m_phkm;
    return SF_OK;
    }
    catch(...){
        last_err.err_type=SF_E_HANDLE;
        last_err.err_level=2;
        strncpy(last_err.err_msg, "FAIL to release the handle", sizeof(last_err.err_msg));
        strncpy(last_err.call_func , "sf_destroy_word_handle", sizeof(last_err.call_func));
        return SF_E_HANDLE;
    }
}


// @brief 给db_handle绑定word_handle
// @param filter_handle db_handle和已初始化的聚类中心(word)的handle
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_link_word_handle( sf_handle_t db_handle, sf_handle_t word_handle){
        imgRetrieval* irP;
        try{
            irP=(imgRetrieval*) db_handle;
        }
        catch(...){
            last_err.err_type=SF_E_HANDLE;
            last_err.err_level=2;
            strncpy(last_err.err_msg, "invalid database handle", sizeof(last_err.err_msg));
            strncpy(last_err.call_func , "sf_link_word_handle", sizeof(last_err.call_func));
            return SF_E_HANDLE;
        }
        Hkms<float>* m_phkm;
        try{
            m_phkm=(Hkms<float>*) word_handle;
        }
        catch(...){
            last_err.err_type=SF_E_HANDLE;
            last_err.err_level=2;
            strncpy(last_err.err_msg, "invalid word handle", sizeof(last_err.err_msg));
            strncpy(last_err.call_func , "sf_link_word_handle", sizeof(last_err.call_func));
            return SF_E_HANDLE;
        }
        irP->m_phkm=m_phkm;
        return SF_OK;
}

err_msg get_last_err(){
    return last_err;
}
