   
// sample test for sdk 27 July, 2016;

#include <opencv2/opencv.hpp>
#include <vector>
#include "pthread_pool.hpp"
#include<fstream>
#include"include_sense_image_search.h"
#include<string>
#include<ctime>


#define NUM_THREADS 1
using namespace std;
using namespace cv;


int main()
{

    // test one image

    sf_handle_t handle=NULL;
    handle= sf_create_image_searcher();
    sf_handle_t db_handle =NULL;
    db_handle =sf_create_image_db();
    sf_handle_t word_handle=NULL;
    word_handle=sf_create_word_handle("word.st");
    std::clock_t t1,t2;
    t1=clock();
    //sf_load_image_db(db_handle, "./invfile.st");
    sf_link_word_handle(db_handle, word_handle);
    t2=clock();


    std::clock_t t3,t4,t5,t6;
    Mat img= cv::imread("./test.jpg");
    sf_search_result* rlt = new sf_search_result();
    int * index =new int();

    t2=clock();

    sf_add_image(db_handle,img.data,SF_PIX_FMT_BGR888,img.cols,img.rows,img.step, index);
    sf_update_weights(db_handle);
    t3=clock();
 //   sf_delete_image(db_handle, *index);

    t4=clock();
 //   sf_update_weights(db_handle);

    t5=clock();
    sf_search_image(handle, db_handle, img.data, SF_PIX_FMT_BGR888, img.cols, img.rows,img.step, *rlt);

    t6=clock();

    if(rlt->value==1) cout<<"test image is detected"<<endl;
    else cout<<"fail to detect the test iamge"<<endl;

    delete index;
    delete rlt;

    sf_destroy_image_db(db_handle);
    sf_destroy_image_searcher(handle);
    sf_destroy_word_handle(word_handle);


    return 0;
}






