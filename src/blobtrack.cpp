#include "cvaux.h"
#include "highgui.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include "Global.h"
#include "serial.h"
#include "translate.h"
#include "gui_model.h"

#define MY_STRNICMP strncasecmp
#define MY_STRICMP strcasecmp

CvCapture*                  pCap = NULL;
CvBlobTrackerAuto*          pTracker = NULL;
IplImage*   gtkMask = NULL;
static pthread_t tracker_thread;
int prev_max = -1;
int prev_max_count = 0;

/* list of FG DETECTION modules */
static CvFGDetector* cvCreateFGDetector0(){return cvCreateFGDetectorBase(CV_BG_MODEL_FGD, NULL);}
static CvFGDetector* cvCreateFGDetector0Simple(){return cvCreateFGDetectorBase(CV_BG_MODEL_FGD_SIMPLE, NULL);}
static CvFGDetector* cvCreateFGDetector1(){return cvCreateFGDetectorBase(CV_BG_MODEL_MOG, NULL);}
typedef struct DefModule_FGDetector
{
    CvFGDetector* (*create)();
    char* nickname;
    char* description;
} DefModule_FGDetector;
DefModule_FGDetector FGDetector_Modules[] =
{
    {cvCreateFGDetector0,"FG_0","Foreground Object Detection from Videos Containing Complex Background. ACM MM2003."},
    {cvCreateFGDetector0Simple,"FG_0S","Simplyfied version of FG_0"},
    {cvCreateFGDetector1,"FG_1","Adaptive background mixture models for real-time tracking. CVPR1999"},
    {NULL,NULL,NULL}
};

/* list of BLOB DETECTION modules */
typedef struct DefModule_BlobDetector
{
    CvBlobDetector* (*create)();
    char* nickname;
    char* description;
} DefModule_BlobDetector;
DefModule_BlobDetector BlobDetector_Modules[] =
{
    {cvCreateBlobDetectorCC,"BD_CC","Detect new blob by tracking CC of FG mask"},
    {cvCreateBlobDetectorSimple,"BD_Simple","Detect new blob by uniform moving of connected components of FG mask"},
    {NULL,NULL,NULL}
};

/* list of BLOB TRACKING modules */
typedef struct DefModule_BlobTracker
{
    CvBlobTracker* (*create)();
    char* nickname;
    char* description;
} DefModule_BlobTracker;
DefModule_BlobTracker BlobTracker_Modules[] =
{
    {cvCreateBlobTrackerCCMSPF,"CCMSPF","connected component tracking and MSPF resolver for collision"},
    {cvCreateBlobTrackerCC,"CC","Simple connected component tracking"},
    {cvCreateBlobTrackerMS,"MS","Mean shift algorithm "},
    {cvCreateBlobTrackerMSFG,"MSFG","Mean shift algorithm with FG mask using"},
    {cvCreateBlobTrackerMSPF,"MSPF","Particle filtering based on MS weight"},
    {NULL,NULL,NULL}
};

/* list of BLOB TRAJECTORY GENERATION modules */
typedef struct DefModule_BlobTrackGen
{
    CvBlobTrackGen* (*create)();
    char* nickname;
    char* description;
} DefModule_BlobTrackGen;
DefModule_BlobTrackGen BlobTrackGen_Modules[] =
{
    {cvCreateModuleBlobTrackGenYML,"YML","Generate track record in YML format as synthetic video data"},
    {cvCreateModuleBlobTrackGen1,"RawTracks","Generate raw track record (x,y,sx,sy),()... in each line"},
    {NULL,NULL,NULL}
};

/* list of BLOB TRAJECTORY POST PROCESSING modules */
typedef struct DefModule_BlobTrackPostProc
{
    CvBlobTrackPostProc* (*create)();
    char* nickname;
    char* description;
} DefModule_BlobTrackPostProc;
DefModule_BlobTrackPostProc BlobTrackPostProc_Modules[] =
{
    {cvCreateModuleBlobTrackPostProcKalman,"Kalman","Kalman filtering of blob position and size"},
    {NULL,"None","No post processing filter"},
//    {cvCreateModuleBlobTrackPostProcTimeAverRect,"TimeAverRect","Average by time using rectangle window"},
//    {cvCreateModuleBlobTrackPostProcTimeAverExp,"TimeAverExp","Average by time using exponential window"},
    {NULL,NULL,NULL}
};

/* list of BLOB TRAJECTORY ANALYSIS modules */
CvBlobTrackAnalysis* cvCreateModuleBlobTrackAnalysisDetector();

typedef struct DefModule_BlobTrackAnalysis
{
    CvBlobTrackAnalysis* (*create)();
    char* nickname;
    char* description;
} DefModule_BlobTrackAnalysis;
DefModule_BlobTrackAnalysis BlobTrackAnalysis_Modules[] =
{
    {cvCreateModuleBlobTrackAnalysisHistPVS,"HistPVS","Histogramm of 5D feture vector analysis (x,y,vx,vy,state)"},
    {NULL,"None","No trajectory analiser"},
    {cvCreateModuleBlobTrackAnalysisHistP,"HistP","Histogramm of 2D feture vector analysis (x,y)"},
    {cvCreateModuleBlobTrackAnalysisHistPV,"HistPV","Histogramm of 4D feture vector analysis (x,y,vx,vy)"},
    {cvCreateModuleBlobTrackAnalysisHistSS,"HistSS","Histogramm of 4D feture vector analysis (startpos,endpos)"},
    {cvCreateModuleBlobTrackAnalysisTrackDist,"TrackDist","Compare tracks directly"},
    {cvCreateModuleBlobTrackAnalysisIOR,"IOR","Integrator (by OR operation) of several analysers "},
    {NULL,NULL,NULL}
};
/* list of Blob Trajectory ANALYSIS modules */
/*================= END MODULES DECRIPTION ===================================*/

// Converts OpenCV image to GTK pixbuf
// Sets pImage to pixbuf containing gtkMask (openCV) image
void convertOpenCv2Gtk(IplImage *image){
    GdkPixbuf *pix;
    cvCvtColor( image, gtkMask, CV_BGR2RGB ); // Usually opencv image is BGR, so we need to change it to RGB
    pix = gdk_pixbuf_new_from_data ((guchar*)gtkMask->imageData,
            GDK_COLORSPACE_RGB,
            FALSE,
            gtkMask->depth,
            gtkMask->width,
            gtkMask->height,
            (gtkMask->widthStep),
            NULL,
            NULL);
    draw_ready = 1;
    gtk_image_set_from_pixbuf( (GtkImage *) pImage, pix );
} 

/* run pipeline on all frames */
static int RunBlobTrackingAuto()
{
    int                     OneFrameProcess = 0;
    int                     key;
    int                     FrameNum = 0;
    CvVideoWriter*          pFGAvi = NULL;
    CvVideoWriter*          pBTAvi = NULL;
        IplImage*   pImg = NULL;

    //cvNamedWindow( "FG", 0 );
        pImg = cvQueryFrame(pCap);
        if(pImg!=NULL){
            gtkMask = cvCreateImage( cvGetSize(pImg), 8, 3);
            convertOpenCv2Gtk(pImg);
            sched_yield();
        }


        //cvNamedWindow( "Tracking", CV_WINDOW_AUTOSIZE );
    /* main cicle */
    for( FrameNum=0; pCap && !quit;
         FrameNum++)
    {/* main cicle */
        IplImage*   pMask = NULL;
        pImg = NULL;

        if(key!=-1)
        {
            OneFrameProcess = 1;
            if(key=='r')OneFrameProcess = 0;
        }

        pImg = cvQueryFrame(pCap);
        //cvShowImage( "Tracking",pImg );
        if(pImg == NULL) break;

		cvCvtColor( pImg, gtkMask, CV_BGR2RGB );

        /* Process */
        pTracker->Process(pImg, pMask);

        if(pTracker->GetFGMask())
        {/* debug FG */
            IplImage*           pFG = pTracker->GetFGMask();
            CvSize              S = cvSize(pFG->width,pFG->height);
            static IplImage*    pI = NULL;

            if(pI==NULL)pI = cvCreateImage(S,pFG->depth,3);
            cvCvtColor( pFG, pI, CV_GRAY2BGR );

            if(pTracker->GetBlobNum()>0)
            {/* draw detected blobs */
                int i;
                int max_size = 0;
                int cur_max = -1;
                int size_i = 0;
                /* Make sure we're in autonomous mode */
                if((pGuiModel->cStatus & MODE) == AUTO){
                    for(i=pTracker->GetBlobNum();i>0;i--)
                    {
                        CvBlob* pB = pTracker->GetBlob(i-1);
                        CvSize  s = cvSize(MAX(1,cvRound(CV_BLOB_RX(pB))), MAX(1,cvRound(CV_BLOB_RY(pB))));
                        size_i = s.width * s.height;
                        if(size_i > max_size){
                            cur_max = CV_BLOB_ID(pB);
                            max_size = size_i;
                        }
                    }/* next blob */;
                    if(cur_max != -1){
                        CvBlob* pB = pTracker->GetBlob(cur_max);
                        if(pB!=NULL){
                            CvPoint p = cvPointFrom32f(CV_BLOB_CENTER(pB));
                            CvSize  s = cvSize(MAX(1,cvRound(CV_BLOB_RX(pB))), MAX(1,cvRound(CV_BLOB_RY(pB))));
                            printf("tracking cur biggest blob %d at: %d,%d size: %dx%d\n",CV_BLOB_ID(pB),p.x,p.y,s.width,s.height);
                            /* ensure this has been the biggest blob for 1 frame before moving, 5 frames before firing at it */
                            if(prev_max != cur_max){
                                prev_max_count = 0;
                                prev_max = cur_max;
                            }else {
                                ++prev_max_count;
                                move_x(x_pix_to_pwm(p.x));
                                printf("x pos (pwm): %d\n",x_pix_to_pwm(p.x));
                                move_y(y_pix_to_pwm(p.y));
                                printf("y pos (pwm): %d\n",y_pix_to_pwm(p.y));
                            }
                            if(prev_max_count > 5){
                                //                            fire();
                            }
                        }
                    }
                }
            }

            cvNamedWindow( "FG",0);
            cvShowImage( "FG",pFG);
        }/* debug FG*/


    }/* main cicle */

    if(pFGAvi)cvReleaseVideoWriter( &pFGAvi );
    if(pBTAvi)cvReleaseVideoWriter( &pBTAvi );
    return 0;
}/* RunBlobTrackingAuto */

/* read parameters from command line and transfer to specified module */
static void set_params(int argc, char* argv[], CvVSModule* pM, char* prefix, char* module)
{
    int prefix_len = strlen(prefix);
    int i;
    for(i=0;i<argc;++i)
    {
        int j;
        char* ptr_eq = NULL;
        int   cmd_param_len=0;
        char* cmd = argv[i];
        if(MY_STRNICMP(prefix,cmd,prefix_len)!=0) continue;
        cmd += prefix_len;
        if(cmd[0]!=':')continue;
        cmd++;

        ptr_eq = strchr(cmd,'=');
        if(ptr_eq)cmd_param_len = ptr_eq-cmd;
        for(j=0;;++j)
        {
            int     param_len;
            char*   param = pM->GetParamName(j);
            if(param==NULL) break;
            param_len = strlen(param);
            if(cmd_param_len!=param_len) continue;
            if(MY_STRNICMP(param,cmd,param_len)!=0) continue;
            cmd+=param_len;
            if(cmd[0]!='=')continue;
            cmd++;
            pM->SetParamStr(param,cmd);
            printf("%s:%s param set to %g\n",module,param,pM->GetParam(param));
        }
    }
    pM->ParamUpdate();
}/* set_params */

/* print all parameters value for given module */
static void print_params(CvVSModule* pM, char* module, char* log_name)
{
    FILE* log = log_name?fopen(log_name,"at"):NULL;
    int i;
    if(pM->GetParamName(0) == NULL ) return;


    printf("%s(%s) module parameters:\n",module,pM->GetNickName());
    if(log)
        fprintf(log,"%s(%s) module parameters:\n",module,pM->GetNickName());
    for(i=0;;++i)
    {
        char*   param = pM->GetParamName(i);
        char*   str = param?pM->GetParamStr(param):NULL;
        if(param == NULL)break;
        if(str)
        {
            printf("  %s: %s\n",param,str);
            if(log)
                fprintf(log,"  %s: %s\n",param,str);
        }
        else
        {
            printf("  %s: %g\n",param,pM->GetParam(param));
            if(log)
                fprintf(log,"  %s: %g\n",param,pM->GetParam(param));
        }
    }
    if(log)fclose(log);
}/* print_params */

void *tracker_thread_function(void * ptr){
    printf("blobtracking thread started\n");
    RunBlobTrackingAuto();
}

int blobtrack_init(void)
{
    CvBlobTrackerAutoParam1     param = {0};

    float       scale = 1;
    char*       scale_name = NULL;
    char*       yml_name = NULL;
    char**      yml_video_names = NULL;
    int         yml_video_num = 0;
    char*       avi_name = NULL;
    char*       fg_name = NULL;
    char*       fgavi_name = NULL;
    char*       btavi_name = NULL;
    char*       bd_name = NULL;
    char*       bt_name = NULL;
    char*       btgen_name = NULL;
    char*       btpp_name = NULL;
    char*       bta_name = NULL;
    char*       bta_data_name = NULL;
    char*       track_name = NULL;
    char*       comment_name = NULL;
    char*       FGTrainFrames = NULL;
    char*       log_name = NULL;
    char*       savestate_name = NULL;
    char*       loadstate_name = NULL;
    char*       bt_corr = NULL;
    DefModule_FGDetector*           pFGModule = NULL;
    DefModule_BlobDetector*         pBDModule = NULL;
    DefModule_BlobTracker*          pBTModule = NULL;
    DefModule_BlobTrackPostProc*    pBTPostProcModule = NULL;
    DefModule_BlobTrackGen*         pBTGenModule = NULL;
    DefModule_BlobTrackAnalysis*    pBTAnalysisModule = NULL;

    draw_ready = 0;

    //cvInitSystem(argc, argv);

    if(track_name)
    {/* set Trajectory Generator module */
        int i;
        if(!btgen_name)btgen_name=BlobTrackGen_Modules[0].nickname;
        for(i=0;BlobTrackGen_Modules[i].nickname;++i)
        {
            if(MY_STRICMP(BlobTrackGen_Modules[i].nickname,btgen_name)==0)
                pBTGenModule = BlobTrackGen_Modules + i;
        }
    }/* set Trajectory Generato modulke */

    /* init postprocessing module if tracker correction by postporcessing is reqierd */
    if(bt_corr && MY_STRICMP(bt_corr,"PostProcRes")!=0 && !btpp_name)
    {
        btpp_name = bt_corr;
        if(MY_STRICMP(btpp_name,"none")!=0)bt_corr = "PostProcRes";
    }

    {/* set default parameters for one processing */
        if(!bt_corr) bt_corr = "none";
        if(!fg_name) fg_name = FGDetector_Modules[0].nickname;
        if(!bd_name) bd_name = BlobDetector_Modules[0].nickname;
        if(!bt_name) bt_name = BlobTracker_Modules[0].nickname;
        if(!btpp_name) btpp_name = BlobTrackPostProc_Modules[0].nickname;
        if(!bta_name) bta_name = BlobTrackAnalysis_Modules[0].nickname;
        if(!scale_name) scale_name = "1";
    }

    if(scale_name) 
        scale = (float)atof(scale_name);
    for(pFGModule=FGDetector_Modules;pFGModule->nickname;++pFGModule)
        if( fg_name && MY_STRICMP(fg_name,pFGModule->nickname)==0 ) break;
    for(pBDModule=BlobDetector_Modules;pBDModule->nickname;++pBDModule)
        if( bd_name && MY_STRICMP(bd_name,pBDModule->nickname)==0 ) break;
    for(pBTModule=BlobTracker_Modules;pBTModule->nickname;++pBTModule)
        if( bt_name && MY_STRICMP(bt_name,pBTModule->nickname)==0 ) break;
    for(pBTPostProcModule=BlobTrackPostProc_Modules;pBTPostProcModule->nickname;++pBTPostProcModule)
        if( btpp_name && MY_STRICMP(btpp_name,pBTPostProcModule->nickname)==0 ) break;
    for(pBTAnalysisModule=BlobTrackAnalysis_Modules;pBTAnalysisModule->nickname;++pBTAnalysisModule)
        if( bta_name && MY_STRICMP(bta_name,pBTAnalysisModule->nickname)==0 ) break;

    pCap = cvCaptureFromCAM( CV_CAP_ANY );
    printf("%f x %f\n", 
            cvGetCaptureProperty(pCap, CV_CAP_PROP_FRAME_WIDTH),
            cvGetCaptureProperty(pCap, CV_CAP_PROP_FRAME_HEIGHT));
    cvSetCaptureProperty(pCap, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(pCap, CV_CAP_PROP_FRAME_HEIGHT, 240);
    printf("%f x %f\n", 
            cvGetCaptureProperty(pCap, CV_CAP_PROP_FRAME_WIDTH),
            cvGetCaptureProperty(pCap, CV_CAP_PROP_FRAME_HEIGHT));
    if(pCap==NULL)
    {
        printf("Can't open %s file\n",avi_name);
        return -1;
    }

    {   /* create autotracker module and its components*/
        param.FGTrainFrames = FGTrainFrames?atoi(FGTrainFrames):0;

        /* Create FG Detection module */
        param.pFG = pFGModule->create();
        if(!param.pFG)
            puts("Can not create FGDetector module");
        param.pFG->SetNickName(pFGModule->nickname);
        //set_params(argc, argv, param.pFG, "fg", pFGModule->nickname);

        /* Create Blob Entrance Detection module */
        param.pBD = pBDModule->create();
        if(!param.pBD)
            puts("Can not create BlobDetector module");
        param.pBD->SetNickName(pBDModule->nickname);
        //set_params(argc, argv, param.pBD, "bd", pBDModule->nickname);

        /* Create blob tracker module */
        param.pBT = pBTModule->create();
        if(!param.pBT)
            puts("Can not create BlobTracker module");
        param.pBT->SetNickName(pBTModule->nickname);
        //set_params(argc, argv, param.pBT, "bt", pBTModule->nickname);

        /* create blob trajectory generation module */
        param.pBTGen = NULL;
        if(pBTGenModule && track_name && pBTGenModule->create)
        {
            param.pBTGen = pBTGenModule->create();
            param.pBTGen->SetFileName(track_name);
        }
        if(param.pBTGen)
        {
            param.pBTGen->SetNickName(pBTGenModule->nickname);
            //set_params(argc, argv, param.pBTGen, "btgen", pBTGenModule->nickname);
        }

        /* create blob trajectory post processing module */
        param.pBTPP = NULL;
        if(pBTPostProcModule && pBTPostProcModule->create)
        {
            param.pBTPP = pBTPostProcModule->create();
        }
        if(param.pBTPP)
        {
            param.pBTPP->SetNickName(pBTPostProcModule->nickname);
            //set_params(argc, argv, param.pBTPP, "btpp", pBTPostProcModule->nickname);
        }

        param.UsePPData = (bt_corr && MY_STRICMP(bt_corr,"PostProcRes")==0);

        /* create blob trajectory analysis module */
        param.pBTA = NULL;
        if(pBTAnalysisModule && pBTAnalysisModule->create)
        {
            param.pBTA = pBTAnalysisModule->create();
            param.pBTA->SetFileName(bta_data_name);
        }
        if(param.pBTA)
        {
            param.pBTA->SetNickName(pBTAnalysisModule->nickname);
            //set_params(argc, argv, param.pBTA, "bta", pBTAnalysisModule->nickname);
        }

        /* create whole pipline */
        pTracker = cvCreateBlobTrackerAuto1(&param);
        if(!pTracker)
            puts("Can not create BlobTrackerAuto");
    }
    printf("finished blob track init\n");
    return pthread_create( &tracker_thread, NULL, tracker_thread_function, NULL);
}

void blobtrack_cleanup(void)
{
    pthread_join( tracker_thread, NULL );
    if(pCap)
        cvReleaseCapture(&pCap);
}

extern "C" {
    int adp_blobtrack_init(void){
        blobtrack_init();
    }
    void adp_blobtrack_cleanup(void){
        blobtrack_cleanup();
    }
}


/*
   int main(int argc, char* argv[])
   {

   blobtrack_init();

   RunBlobTrackingAuto();//pCap, pTracker, fgavi_name, btavi_name );

   blobtrack_cleanup();

   return 0;
   }*/
