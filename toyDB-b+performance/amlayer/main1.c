#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


#include <stdio.h>
#include "am.h"
#include "pf.h"
#include "math.h"
#include "testam.h"
#include "time.h"
#define MAXRECS	20000	/* max # of records to insert */
#define FNAME_LENGTH 80	/* file name size */

extern int buffaccesses,buffmisses;
int sum_keys=0;
int sum_max_keys=0;

int occ_space=0;

void resetglobals(void)
{sum_keys=0;sum_max_keys=0;occ_space=0;}

void resetbuffstatus(void)
{buffaccesses=0;buffmisses=0;}

int AM_numnodes(int fileDesc,int* pagenum,int attriblength)
{
	char* pageBuf;
	int errVal = PF_GetThisPage(fileDesc,*pagenum,&pageBuf);
	//printf("errval : %d\n",err);
	
	AM_Check;
	
	if ( *pageBuf == 'l' ){
	AM_LEAFHEADER head;
	AM_LEAFHEADER *header;
	header=&head;
	int recordsize= attriblength+ AM_ss;
	bcopy(pageBuf,header,AM_sl);
	sum_keys+=header->numKeys;
	sum_max_keys+=header->maxKeys;
	occ_space+=recordsize*header->numKeys+AM_sl;
	errVal = PF_UnfixPage(fileDesc,*pagenum,FALSE);
	AM_Check;		
	return 1;}

	else
	{		
		AM_INTHEADER head;
		AM_INTHEADER *header;
		header=&head;
		bcopy(pageBuf,header,AM_sint); 
		int i,nextpagenum;
		int sum=0,recordsize = attriblength+ AM_si;
		sum_keys+=header->numKeys;
		sum_max_keys+=header->maxKeys;
		occ_space+= recordsize*header->numKeys+AM_si+AM_sint;
		for( i=0;i<=header->numKeys;i++)
		{bcopy(pageBuf+AM_sint+i*recordsize,(char *)&nextpagenum,
		              AM_si);
		 
		sum += AM_numnodes( fileDesc,&nextpagenum,attriblength);
		}
		errVal = PF_UnfixPage(fileDesc,*pagenum,FALSE);
		AM_Check;
		return 1+sum;
		
	}
}

int AM_numlevels(int fileDesc,int *pagenum,int attriblength)
{
	char* pageBuf;
	int errVal = PF_GetThisPage(fileDesc,*pagenum,&pageBuf);
	AM_Check;
	 
	if ( *pageBuf == 'l' ){ errVal = PF_UnfixPage(fileDesc,*pagenum,FALSE);
		AM_Check;return 1;}
	else
	{		
		AM_INTHEADER head;
		AM_INTHEADER *header;
		header=&head;
		bcopy(pageBuf,header,AM_sint);
		int i,nextpagenum;
		int levels=0,recordsize = attriblength+ AM_si;
		for( i=0;i<=header->numKeys;i++)
		{bcopy(pageBuf+AM_sint+i*recordsize,(char *)&nextpagenum,
		              AM_si);
		levels= max(AM_numlevels( fileDesc,&nextpagenum,attriblength),levels);
		}
		errVal = PF_UnfixPage(fileDesc,*pagenum,FALSE);
		AM_Check;
		return 1+levels;
		
	}
}

main()
{
int fd;	/* file descriptor for the index */
char fname[FNAME_LENGTH];	/* file name */
int recnum;	/* record number */
int sd;	/* scan descriptor */
int numrec;	/* # of records retrieved */
int testval;	
int i;
int list[2000];							//generate random numbers
for (i = 0; i < 2000; i++) {
    list[i] = i;
}
for ( i = 0; i < 2000; i++) {
    int j = i + rand() % (2000 - i);
    int temp = list[i];
    list[i] = list[j];
    list[j] = temp;
}

	/* init */
	printf("initializing\n");
	PF_Init();

	/* create index */
	printf("creating index\n");
	AM_CreateIndex(RELNAME1,0,INT_TYPE,sizeof(int));
	
	/* open the index */
	printf("opening index\n");
	sprintf(fname,"%s.0",RELNAME1);
	fd = PF_OpenFile(fname);

	
	/* first, make sure that simple deletions work */
	printf("inserting into index\n");
	clock_t start = clock();
   //... do work here
   
	for (recnum=0; recnum < 2000; recnum++){
		AM_InsertEntry(fd,INT_TYPE,sizeof(int),(char *)&list[recnum],
				list[recnum]);
	}
	clock_t end = clock();
	double insertion_time = (end - start)/(double)CLOCKS_PER_SEC;
	//printf("%d \n", AM_RootPageNum);
	// char * testbuf;
	// int errVal = PF_GetThisPage(fd,AM_RootPageNum,&testbuf);
	// printf("err val is %d \n",errVal);
	

	int numnodes= AM_numnodes(fd,&AM_RootPageNum,sizeof(int));
	int numlevels= AM_numlevels(fd,&AM_RootPageNum,sizeof(int));
	double hit_ratio= (double)(buffaccesses-buffmisses)/buffaccesses;
	double space_utilisation= (double)100*occ_space/(numnodes*PF_PAGE_SIZE);
	printf("no of nodes= %d\nno of levels=%d\ntotal occcupied space(in bytes)=%d\ntotal allocated space(in bytes)=%d\n space-ultilisationpercent = %f" ,numnodes,numlevels,occ_space,numnodes*PF_PAGE_SIZE,space_utilisation);
	printf("buffaccesses= %d \n buffmisses= %d\n insertion buffer hit-ratio=%f\n insertion time=%f\n",buffaccesses,buffmisses,hit_ratio,insertion_time);
	
	resetglobals();resetbuffstatus();
	
	
	printf("Giving some random values to search..\n");
	int searchvalues[5]={30,76,267,401,971};
	double searchtimes[5];
	
	for(i=0;i<5;i++)
	{
		int pageNum;char* pageBuf;int indexPtr;
	start = clock();
	int searchval= AM_Search(fd,'i',4,(char *)&searchvalues[i],&pageNum,&pageBuf,&indexPtr);
	end	=	clock();
	searchtimes[i] = (end - start)/(double)CLOCKS_PER_SEC;
	printf("Value %d found at page-number: %d , index-number: %d\n",searchvalues[i],pageNum,indexPtr);
	}
	double avg_searchtime=0;
	for(i=0;i<5;i++)
	{avg_searchtime+=searchtimes[i]/5;}
	hit_ratio=(double)(buffaccesses-buffmisses)/buffaccesses;
	printf("buffaccesses= %d \n buffmisses= %d\n deletion buffer hit-ratio=%f\n average search time=%f\n",buffaccesses,buffmisses,hit_ratio,avg_searchtime);
	
	resetglobals();resetbuffstatus();
	
	
	
	
	printf("deleting records between 2000-600\n");
	start = clock();
	 for (recnum=2000; recnum < 600; recnum ++)
	 {	int errVal=AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
	 				recnum);
	 	
	 }
	 end	=	clock();
	double deletion_time = (end - start)/(double)CLOCKS_PER_SEC;
	hit_ratio=(double)(buffaccesses-buffmisses)/buffaccesses;
	printf("buffaccesses= %d \n buffmisses= %d\n search buffer hit-ratio=%f\n deletion time=%f\n",buffaccesses,buffmisses,hit_ratio,deletion_time);
	 

	/* destroy everything */
	printf("closing down\n");
	PF_CloseFile(fd);
	AM_DestroyIndex(RELNAME1,0);
	

	
}
