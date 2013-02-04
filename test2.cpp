//#include "stdafx.h"
 
#include "cv.h"
#include "highgui.h"
#include <ctype.h>
#include <cmath>
#include <stdio.h>
#include <vector>
using std::vector;

#include <new>


#include <iostream>
using std::cout;
using std::endl;

#include <stdarg.h>
#include <time.h>

void cvShowManyImages(char* title, int nArgs, ...) 
{

   // img - Used for getting the arguments 
   IplImage *img;

   // DispImage - the image in which input images are to be copied
   IplImage *DispImage;

   int size;
   int i;
   int m, n;
   int x, y;

   // w - Maximum number of images in a row 
   // h - Maximum number of images in a column 
   int w, h;

   // scale - How much we have to resize the image
   float scale;
   int max;

   // If the number of arguments is lesser than 0 or greater than 12
   // return without displaying 
   if(nArgs <= 0) {
      printf("Number of arguments too small....\n");
      return;
   }
   else if(nArgs > 12) {
      printf("Number of arguments too large....\n");
      return;
   }
   // Determine the size of the image, 
   // and the number of rows/cols 
   // from number of arguments 
   else if (nArgs == 1) {
      w = h = 1;
      size = 300;
   }
   else if (nArgs == 2) {
      w = 2; h = 1;
      size = 300;
   }
   else if (nArgs == 3 || nArgs == 4) {
      w = 2; h = 2;
      size = 300;
   }
   else if (nArgs == 5 || nArgs == 6) {
      w = 3; h = 2;
      size = 200;
   }
   else if (nArgs == 7 || nArgs == 8) {
      w = 4; h = 2;
      size = 200;
   }
   else {
      w = 4; h = 3;
      size = 150;
   }

   // Create a new 3 channel image
   DispImage = cvCreateImage( cvSize(100 + size*w, 60 + size*h), 8, 3 );

   // Used to get the arguments passed
   va_list args;
   va_start(args, nArgs);

   // Loop for nArgs number of arguments
   for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {

      // Get the Pointer to the IplImage
      img = va_arg(args, IplImage*);

      // Check whether it is NULL or not
      // If it is NULL, release the image, and return
      if(img == 0) {
         printf("Invalid arguments");
         cvReleaseImage(&DispImage);
         return;
      }

      // Find the width and height of the image
      x = img->width;
      y = img->height;

      // Find whether height or width is greater in order to resize the image
      max = (x > y)? x: y;

      // Find the scaling factor to resize the image
      scale = (float) ( (float) max / size );

      // Used to Align the images
      if( i % w == 0 && m!= 20) {
         m = 20;
         n+= 20 + size;
      }

      // Set the image ROI to display the current image
      cvSetImageROI(DispImage, cvRect(m, n, (int)( x/scale ), (int)( y/scale )));

      // Resize the input image and copy the it to the Single Big Image
      cvResize(img, DispImage);

      // Reset the ROI in order to display the next image
      cvResetImageROI(DispImage);
   }

   // Create a new window, and show the Single Big Image
   //cvNamedWindow( title, 1 );
   cvShowImage( title, DispImage);

   /*cvWaitKey(0);*/
   //cvDestroyWindow(title);

   // End the number of arguments
   va_end(args);

   // Release the Image Memory
   cvReleaseImage(&DispImage);
}

struct Object
{
	int startX;  //最左边的点的X值
	int endX;    //最右边的点的X值
	int startY;  //最上边的点的Y值
	int endY;    //最下面的点的Y值
	int ID;
	bool is_deleted;
	int area;
};


int main(int argc, char** argv)
{
	CvPoint shoulder1,shoulder2;
	CvPoint arm1,arm2;
	CvPoint hand1,hand2;
	CvPoint ass1,ass2;
	CvPoint knee1,knee2;
	CvPoint foot1,foot2;
	CvPoint chest;
	CvPoint abdomen; //小腹
	int key;
	int i,j,k,a,b,t1,t2,temp1;
	int temp;        //用于判断当前节点
	int ObjectNumber;   //当前帧下区域总数
	int height=0;
	int width=0;
	int step=0;
	int channels=0;
	int count=0;        //判断首帧
//	IplImage* containbox=0;
	IplImage* gray=0;		
	IplImage* circles=0; 
	IplImage* frame =0;
	IplImage* frame1 =0;
	uchar* data=NULL;   //图像数据
	uchar* address=NULL;//图像数据的地址
    int* LineState;     //用于存放每行的状态
	int NumberOfArea=0; //当前目标数
	vector<Object> FrameObject;//当前帧的目标
	CvScalar color=CV_RGB(0,0,255);//这同样是测试用的画点的颜色
	CvCapture* capture=0,* capture2=0;
	int sort[14];           //最短距离法要用到

 //   if(argc ==1 || (argc ==2 && strlen(argv[1]) ==1 && isdigit(argv[1][0])))  //无参数或者是纯数字（指定摄像头的标号）
 //       capture = cvCaptureFromCAM(argc ==2? argv[1][0]-'0':0);
    capture = cvCreateFileCapture(argv[1]);
	capture2 = cvCreateFileCapture(argv[1]);
	if(!capture)
	{
		fprintf(stderr,"Could not initialize capturing...\n");
		return -1;
	}
	cvNamedWindow("Mocap",1);
	cvResizeWindow("Mocap",700,660);
//	cvNamedWindow("Containbox",0);
//	cvNamedWindow("Binary",0);
    while(1)
	 {
	  count++;
	  Object t_Object;
	  t_Object.is_deleted=false;
	  t_Object.area=0;
	  FrameObject.push_back(t_Object); //这句非加不可，不知道为什么
	  frame=    cvQueryFrame(capture);
	  frame1=    cvQueryFrame(capture2);
	  if(!frame)
			break;
//      frame=    cvQueryFrame(capture);
	  data  =(uchar*)frame->imageData;

/*	if(!capture)
	{
		fprintf(stderr,"Could not initialize capturing...\n");
		return -1;
	}
*/
  
    height=frame->height;
	width =frame->width;
	step  =frame->widthStep;
	channels=frame->nChannels;
	gray=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	circles=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
//	containbox=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);

		for(i=0;i<height;i++)
			for(j=0;j<width;j++)
				if(data[i*step+j*channels+2]-data[i*step+j*channels]<70)
				{
					data[i*step+j*channels]= 0;
                    data[i*step+j*channels+1]=0;
					data[i*step+j*channels+2]=0;
				}
		
	    LineState=new int[width];
	    for(i=0;i<width;i++)
		   LineState[i]=-1;
        
				
        cvCvtColor(frame,gray,CV_RGB2GRAY);
		cvThreshold(gray,gray,0,255,CV_THRESH_OTSU);

//		cvFlip(gray,gray,0);    //图像倒置

//		cvSmooth(gray,gray,2,5);
		cvDilate(gray,gray,0,1);//图像膨胀

        address=(uchar*)&(gray->imageData[0]);
		
		//-----------------------------寻找中心---------------------------------------
		//----------------------------------------------------------------------------
		for ( j = 0; j < height ; j++ )
		{
		
		     k = 0;
			 	
             for ( i = 0; i < width; i++ )
			 {
			         if ( (* address) ==0 )		//如果该点灰度小于门限，则状态位置0
					 {
				          LineState[k] = 0;
					 }
			         else		//如果灰度高于门限，则归并区域
					 {
						  
				        if ( LineState[k] > 0 )		//如果上一行该点属于某一区域，则该点并入该区域
						{   
						   
                            FrameObject[LineState[k]].area++;
					        FrameObject[LineState[k]].endY = j;		//图像的高度修改为当前高度
						}
				        else
						{
							
					        if ( k == 0 )		//如果是图像最左边的点
							{
						        NumberOfArea++;		//区域计数+1

						        //添加新的区域
						        t_Object.endX = i;
						        t_Object.startX = i;
						        t_Object.endY = j;
						        t_Object.startY = j;
						        t_Object.ID = NumberOfArea;
								t_Object.area=1;
					        	FrameObject.push_back( t_Object );

						        //修改当前点状态
						        LineState[k] = NumberOfArea;
							}
					        else		//如果不是最左边的点，则判断左边的点的状态
							{
								
						        if ( LineState[k-1] != 0 )		//如果该点左侧的点状态不为0，则并入该区域
								{ 
								
                                    //修改当前点状态
							        LineState[k] = LineState[k-1];

							        //修改区域信息
                                    FrameObject[LineState[k]].area++;
							        if ( k > FrameObject[LineState[k]].endX )	//如果该点超出右边界，则修改区域右边界
									{
								        FrameObject[LineState[k]].endX = k;
									}
								}
						        else		//如果左侧点状态为0，则添加新的区域
								{
							        NumberOfArea++;		//区域计数+1

							        //添加新的区域
							        t_Object.endX = i;
							        t_Object.startX = i;
							        t_Object.endY = j;
							        t_Object.startY = j;
									t_Object.area=1;
							        t_Object.ID = NumberOfArea;

							        FrameObject.push_back( t_Object );

							        //修改当前点状态
							        LineState[k] = NumberOfArea;
								}
							}
						}
					 }

			    k++;
			    address++;
			 }  //对应于i的那个for的结束
             
		//第二步，每行结束的时候，合并相连的区域
            for ( i = 1; i < width; i++ )
			{
			     if ( ( LineState[i] != LineState[i-1] ) && ( LineState[i] != 0 ) && ( LineState[i-1] != 0 ) )
				 {	 
				   a = LineState[i];
				   b = LineState[i-1];
				   //如果相邻的两个点状态都不为0，且互不相同，则合并两个区域，将第二个区域合并到第一个区域中去
				   //一，确定边界,累加面积
				   if ( FrameObject[a].endX < FrameObject[b].endX )
				   {
					   FrameObject[a].endX = FrameObject[b].endX;
				   }
				   if ( FrameObject[a].startX > FrameObject[b].startX )
				   {
					   FrameObject[a].startX = FrameObject[b].startX;
				   }
				   if ( FrameObject[a].endY < FrameObject[b].endY )
				   {
					   FrameObject[a].endY = FrameObject[b].endY;
				   }
				   if ( FrameObject[a].startY > FrameObject[b].startY )
				   {
					   FrameObject[a].startY =FrameObject[b].startY;
				   }
                   
				   FrameObject[a].area+= FrameObject[b].area;

                   
				//二，删除部分
				   FrameObject[b].is_deleted = true;

				//三，修改状态
				   for ( k = 0; k < width; k++ )
				   {
					  if ( LineState[k] == b )
					  {
						LineState[k] = a;
					  }
				   }
				 }//if结束
			}     //第二次for结束
	 }  //最外层的for结束
	 delete[] LineState;		//清空临时状态队列	 
	 ObjectNumber = FrameObject.size();
	 for ( i = 0; i < ObjectNumber; i++ )
	{
		if ( FrameObject[i].is_deleted == false && FrameObject[i].area > 70)		//如果为已删除区域,则删除，不显示
		{
			//--------这里是显示代码-----------
/*          cvRectangle(circles,
				      cvPoint(FrameObject[i].startX,FrameObject[i].startY),
                      cvPoint(FrameObject[i].endX,FrameObject[i].endY),
					  color,
					  -1,
					  NULL,
					  0);
					  */
            cvCircle(circles,
				     cvPoint((FrameObject[i].startX+FrameObject[i].endX)/2,(FrameObject[i].startY+FrameObject[i].endY)/2),
					 5,
					 color,
					 -1,
					 NULL,
					 0);
			if(count==1) //这是需要标定的时刻
			{
                 cvShowImage("Origin",frame1);
			  // cvShowImage("Binary",frame);
		      // cvShowImage("Containbox",containbox);
                 cvShowImage("Circles",circles);
                 key=cvWaitKey(0);
				 switch(key) {
				 case '1':
                     shoulder1.x= (FrameObject[i].startX+FrameObject[i].endX)/2;
                     shoulder1.y= (FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
                 case '2':
                     shoulder2.x= (FrameObject[i].startX+FrameObject[i].endX)/2;
                     shoulder2.y= (FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case '3':
                     arm1.x= (FrameObject[i].startX+FrameObject[i].endX)/2;
                     arm1.y= (FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case '4':
					 arm2.x= (FrameObject[i].startX+FrameObject[i].endX)/2;
					 arm2.y= (FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
                 //----add code here------
				 case '5':
					 hand1.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
					 hand1.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
                     break;
				 case '6':
                     hand2.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
					 hand2.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case '7':
                     ass1.x= (FrameObject[i].startX+FrameObject[i].endX)/2;
					 ass1.y= (FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case '8':
                     ass2.x= (FrameObject[i].startX+FrameObject[i].endX)/2;
					 ass2.y= (FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case '9':
                     knee1.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
					 knee1.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case '0':
                     knee2.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
					 knee2.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case 'l':
                     foot1.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
                     foot1.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
                     break;
				 case 'r':
                     foot2.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
                     foot2.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
                     break;
				 case 'c':
                     chest.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
					 chest.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 case 'a':
                     abdomen.x=(FrameObject[i].startX+FrameObject[i].endX)/2;
                     abdomen.y=(FrameObject[i].startY+FrameObject[i].endY)/2;
					 break;
				 default:
					 cout<<"你可能还没有完成标定~~"<<endl;
					 break;
				 //-----------------------
				 }			
			cvDestroyWindow("Circles");
			cvDestroyWindow("Origin");
			} //if结束
			else
			{ //判断点的归属
			  //用最短距离法判断点
			  //如果出现自遮挡，该怎么办呢？

			  temp=100;
			  
              t1=(FrameObject[i].startX+FrameObject[i].endX)/2;
			  t2=(FrameObject[i].startY+FrameObject[i].endY)/2;
			  sort[0]=abs(t1-shoulder1.x)+abs(t2-shoulder1.y); 
			  sort[1]=abs(t1-shoulder2.x)+abs(t2-shoulder2.y);
			  sort[2]=abs(t1-arm1.x)+abs(t2-arm1.y);
			  sort[3]=abs(t1-arm2.x)+abs(t2-arm2.y);
			  sort[4]=abs(t1-hand1.x)+abs(t2-hand1.y);
			  sort[5]=abs(t1-hand2.x)+abs(t2-hand2.y);
			  sort[6]=abs(t1-ass1.x)+abs(t2-ass1.y);
			  sort[7]=abs(t1-ass2.x)+abs(t2-ass2.y);
			  sort[8]=abs(t1-knee1.x)+abs(t2-knee1.y);
			  sort[9]=abs(t1-knee2.x)+abs(t2-knee2.y);
			  sort[10]=abs(t1-foot1.x)+abs(t2-foot1.y);
			  sort[11]=abs(t1-foot2.x)+abs(t2-foot2.y);
			  sort[12]=abs(t1-chest.x)+abs(t2-chest.y);
			  sort[13]=abs(t1-abdomen.x)+abs(t2-abdomen.y);
			  for(j=0;j<14;j++)
			  {
				  if(sort[j]<temp)
				  {
					  temp=sort[j];
					  temp1=j;
					 
				  }
			  }
			  switch(temp1) {
				 case 0:
                     shoulder1.x= t1;
                     shoulder1.y= t2;
					 break;
                 case 1:
                     shoulder2.x= t1;
                     shoulder2.y= t2;
					 break;
				 case 2:
                     arm1.x= t1;
                     arm1.y= t2;
					 break;
				 case 3:
					 arm2.x= t1;
					 arm2.y= t2;
					 break;
				 case 4:
					 hand1.x=t1;
					 hand1.y=t2;
                     break;
				 case 5:
                     hand2.x=t1;
					 hand2.y=t2;
					 break;
				 case 6:
                     ass1.x= t1;
					 ass1.y= t2;
					 break;
				 case 7:
                     ass2.x= t1;
					 ass2.y= t2;
					 break;
				 case 8:
                     knee1.x=t1;
					 knee1.y=t2;
					 break;
				 case 9:
                     knee2.x=t1;
					 knee2.y=t2;
					 break;
				 case 10:
                     foot1.x=t1;
                     foot1.y=t2;
                     break;
				 case 11:
                     foot2.x=t1;
                     foot2.y=t2;
                     break;
				 case 12:
                     chest.x=t1;
					 chest.y=t2;
					 break;
				 case 13:
                     abdomen.x=t1;
                     abdomen.y=t2;
					 break;
				 default:
					 cout<<"出错了！！	"<<endl;
					 break;
				  }//switch结束

             
			
			}//else结束
		/*	cvRectangle(containbox,
				      cvPoint(FrameObject[i].startX,FrameObject[i].startY),
                      cvPoint(FrameObject[i].endX,FrameObject[i].endY),
					  color,
					  -1,
					  NULL,
					  0);*/
		    //	cout<<"                ("<<(FrameObject[i].startX+FrameObject[i].endX)/2<<","<<(FrameObject[i].startY+FrameObject[i].endY)/2<<");"<<endl<<endl;
			//--------显示代码结束  -----------
		}//if结束
	}
		//-----------------------
		//-----寻找中心结束------
	    NumberOfArea=0;  //一帧结束 清零
        FrameObject.clear();
//		delete address;
//		delete data;
//		cout<<count<<endl;

		
//		cvShowImage("Binary",frame);
//		cvShowImage("Containbox",containbox);
       
/*			cvLine(circles,shoulder1,shoulder2,color,3,CV_AA,0);
			cvLine(circles,shoulder1,arm1,color,3,CV_AA,0);
			cvLine(circles,shoulder2,arm2,color,3,CV_AA,0);
			cvLine(circles,arm1,hand1,color,3,CV_AA,0);
			cvLine(circles,arm2,hand2,color,3,CV_AA,0);
			cvLine(circles,chest,abdomen,color,3,CV_AA,0);
			cvLine(circles,ass1,ass2,color,3,CV_AA,0);
			cvLine(circles,ass1,knee1,color,3,CV_AA,0);
			cvLine(circles,ass2,knee2,color,3,CV_AA,0);
			cvLine(circles,knee1,foot1,color,3,CV_AA,0);
			cvLine(circles,knee2,foot2,color,3,CV_AA,0);//画线
            cvShowImage("Circles",circles);*/
				
            cvLine(circles,shoulder1,shoulder2,color,3,CV_AA,0);
			cvLine(circles,shoulder1,arm1,color,3,CV_AA,0);
			cvLine(circles,shoulder2,arm2,color,3,CV_AA,0);
			cvLine(circles,arm1,hand1,color,3,CV_AA,0);
			cvLine(circles,arm2,hand2,color,3,CV_AA,0);
			cvLine(circles,chest,abdomen,color,3,CV_AA,0);
			cvLine(circles,ass1,ass2,color,3,CV_AA,0);
			cvLine(circles,ass1,knee1,color,3,CV_AA,0);
			cvLine(circles,ass2,knee2,color,3,CV_AA,0);
			cvLine(circles,knee1,foot1,color,3,CV_AA,0);
			cvLine(circles,knee2,foot2,color,3,CV_AA,0);
			cvLine(circles,chest,cvPoint((shoulder1.x+shoulder2.x)/2,(shoulder1.y+shoulder2.y)/2),color,3,CV_AA,0);
			cvLine(circles,abdomen,cvPoint((ass1.x+ass2.x)/2,(ass1.y+ass2.y)/2),color,3,CV_AA,0);//画线
            cvCircle(circles,
				     cvPoint((shoulder1.x+shoulder2.x)/2,(shoulder1.y+shoulder2.y)/2-15),
					 15,
					 color,
					 -1,
					 NULL,
					 0);
			cvShowManyImages("Mocap",2,frame1,circles);

	        if(cvWaitKey(10)>=0)
			      break;
 
   
	
    }
	cvReleaseCapture(&capture);
	cvReleaseCapture(&capture2);
	cvDestroyWindow("Mocap");
	
	


	return 0;
}
