/* 
 * s3.cpp:
 * The stand-alone S3 program.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "aftimer.h"

#ifdef HAVE_ERR_H
#include "err.h"
#endif

#ifdef USE_S3
#include "s3_glue.h"
char *opt_bucket = 0;
int opt_flag  = 0;
int opt_meta = 0;

using namespace s3;

#define BANDWIDTH_PREFIX ".bandwidth_test"
#define BANDWIDTH_DEFAULT_SIZE 1000000
#define BANDWIDTH_COUNT 3

/* Add commas
 */
const char *af_commas(char buf[64],long long val)
{
    char tmp[64];
    char t2[64];
    int  negative = 0;

    buf[0] = 0;
    if(val==0){
	strcpy(buf,"0");
    }
    if(val<0){
	negative = 1;
	val = -val;
    }

    while(val>0){
	int digits = val % 1000;		// get the residue
	val = val / 1000;		// and shift what's over

	if(val>0){			// we will still have more to do
	    sprintf(tmp,",%03d",digits);	// so pad it with zeros and a comma
	}
	else {
	    sprintf(tmp,"%d",digits);	// otherwise just get the value
	}
	strcpy(t2,buf);			// copy out the buffer
	strcpy(buf,tmp);		// copy in what we just did
	strcat(buf,t2);			// and put back what was there
    }
    if(negative){
	strcpy(t2,buf);
	strcpy(buf,"-");
	strcat(buf,t2);
    }
    return buf;
}

void s3_df()
{
    class s3_result *e = list_buckets();
    if(!e->lambr) errx(1,"S3 did not return ListAllMyBucketsResult.");
    printf("Owner ID: %s\n",e->lambr->OwnerID.c_str());
    printf("Owner Display Name: %s\n",e->lambr->OwnerDisplayName.c_str());
    printf("\n");
    for(vector<Bucket *>::const_iterator i = e->lambr->Buckets.begin();
	i != e->lambr->Buckets.end();
	i++){
	printf("%s %s\n",(*i)->CreationDate.c_str(),(*i)->Name.c_str());
    }
    exit(0);
}

typedef vector <Contents> cvector;
void s3_ls(FILE *out,const char *prefix,cvector *cv)
{
    unsigned long long total=0;
    string bucket = opt_bucket;
    if(out) fprintf(out,"S3 BUCKET %s:",bucket.c_str());
    if(strlen(prefix)>0){
	if(out) fprintf(out,"PREFIX %s",prefix);
    }
    if(out) fprintf(out,"\n\n");
    string marker;
    class s3_result  *e;
    bool isTruncated = false;
    do {
	e = list_bucket(bucket,prefix,marker,00);
	if(e==0) err(1,"Error loading bucket.");
	if(e->lbr==0) err(1,"Error: no LBR");
	if(e->lbr->contents.size()==0){
	    delete e;
	    break;			//
	}
	for(vector<Contents *>::const_iterator i = e->lbr->contents.begin();
	    i != e->lbr->contents.end();
	    i++){

	    if(cv) cv->push_back(**i);
	    /* Make date nice */
	    char tstamp[64];
	    strcpy(tstamp,(*i)->LastModified.c_str());
	    tstamp[10] = ' ';
	    tstamp[19] = '\000';
	    
	    if(out){
		fprintf(out,"%s ",(*i)->OwnerDisplayName.c_str());
		fprintf(out,"%8d  ",(int)(*i)->Size);
		fprintf(out,"%s ",tstamp);
		fprintf(out,"%s ",(*i)->Key.c_str());
		fprintf(out,"\n");
	    }
	    total += (*i)->Size;
	}

	/* "To get the next page of results use the last key of the
	 *  current page as the marker."
	 */

	marker = e->lbr->contents.back()->Key;
	isTruncated = e->lbr->IsTruncated;
	delete e;
    } while(isTruncated);

    char buf[64];
    if(out) fprintf(out,"Total: %s\n",af_commas(buf,total));
}

void s3_cat(string key)
{
    class response_buffer *b = 0;
    if(opt_meta==0){
	b = object_get(opt_bucket,key,0);
    }
    else {
	b = object_head(opt_bucket,key,0);
    }
    if(!b) errx(1,"HTTP transport error");
    if(b->result == 404) errx(1,"S3: %s not found",key.c_str());
    fwrite(b->base,1,b->len,stdout);
    delete b;
}

/* s3 doesn't give an error if you try to delete an object that doesn't exist */
void s3_rm(int argc,char **argv)
{
  argc--;argv++;
    while(*argv){
	printf("s3 rm %s\n",*argv);
	int r = object_rm(opt_bucket,*argv);
	if(r) errx(1,"HTTP transport error");
	argv++;
	argc--;
    }
    exit(0);
}

/* del prefix */
void s3_delp(int argc,char **argv)
{
  argc--;argv++;
  
    char line[80];
    cvector cv;
    if(argc!=1) errx(1,"delp requires a single argument");
    s3_ls(stdout,*argv,&cv);
    if(cv.size()==0) errx(0,"No items to delete");
    printf("Really delete %d item%s?\n",cv.size(),cv.size()==1 ? "" : "s");
    fgets(line,sizeof(line),stdin);
    if(line[0]!='y' && line[0]!='Y') errx(1,"Aborted");
    for(cvector::iterator i=cv.begin();
	i!=cv.end();
	i++){
	printf("s3 rm %s\n",i->Key.c_str());
	if(object_rm(opt_bucket,i->Key.c_str())){
	    warn("HTTP error");
	}
    }
}

void s3_cp(const char *fname,string key)
{
    struct s3headers meta[2] = {{0,0},{0,0}};
    char buf[64];

    if(opt_flag){
	snprintf(buf,sizeof(buf),"%d",opt_flag);
	meta[0].name = AMAZON_METADATA_PREFIX "arg";
	meta[0].value = buf;
    }

    /* Read from fname into a buffer.
     * Note that we do this with read, so that we can read from stdin
     */
    FILE *f = fopen(fname,"r");
    if(!f) err(1,fname);
    class response_buffer inbuf;
    while(!feof(f)){
	char buf[65536];
	int  count;
	count = fread(buf,1,sizeof(buf),f);
	if(count>0){
	    inbuf.write(buf,count);
	}
    }
    if(object_put(opt_bucket,key,inbuf.base,inbuf.len,meta)){
	errx(1,"%s: ",fname);
    }
    exit(0);
}

void s3_mkdir(int argc,char **argv)
{
  argc--;argv++;
    while(argc>0){
	if(bucket_mkdir(*argv)) err(1,"%s",*argv);
	argc--;
	argv++;
    }
    exit(0);
}

void s3_rmdir(string bucket)
{
    if(bucket_rmdir(bucket)) err(1,"%s",bucket.c_str());
    exit(0);
}



void usage()
{
    printf("s3 testing program.\n\n");
    printf("Usage:\n");
    printf("s3 ls (or dir) [prefix] - list the bucket's contents\n");
    printf("s3 mkdir            - make a bucket\n");
    printf("s3 rmdir            - delete a bucket\n");
    printf("s3 df               - display all of the buckets\n");
    printf("s3 rm key           - delete key from the bucket\n");
    printf("s3 cat key          - send the contents of the key to stdout\n");
    printf("s3 cp fname key     - copy local file fname to key\n");
    printf("s3 delp prefix      - Delete all keys with 'prefix' as a prefix\n");
    printf("\n");
    printf("Debugging commands:\n");
    printf("s3 regress          - run regression tests\n");
    printf("s3 bandwidth [-m]   - measure bandwidth (defaults to 'bandwidth' bucket)\n");
    printf("\n");
    printf("Options:\n");
    printf("   -d = enable HTTP debugging\n");
    printf("   -b <bucket>  = Specifies bucket\n");
    printf("   -fnn = specify a 32-bit metadata 'flag'\n");
    printf("   -m = just report the metadata (for cat)\n");
    exit(0);
}

/****************************************************************/

/* Regression testing */
int regress()
{
    /* Make some data */
    for(int i=0;i<100;i++){
	char name[1024];
	char value[1024];
	sprintf(name,"bucket%d",i);
	sprintf(value,"This is the contents of bucket %d\n",i);
	object_put(opt_bucket,name,value,strlen(value),0);
    }
	
    for(int i=0;i<100;i++){
	if(i%10==0) printf("\n");
	class s3_result  *e = list_bucket(opt_bucket,"","",0);
	if(!e->lbr) err(1,"Error loading bucket pass %d\n",i);
	delete e;
	printf("%d ",i);
	fflush(stdout);
    }
    printf("Done. Check for memory leaks, then press any key...\n");
    getchar();
    exit(0);
}

void s3_bandwidth(int argc,char **argv)
{
    int opt_make = opt_meta;	// in case it was set
    int read_retry=0;
    int write_retry=0;
    int write_err=0;

    argc--;argv++;

    if(argc>0 && strcmp(argv[0],"-m")==0){
	opt_make = 1;
	argc--;
	argv++;
    }

    if(opt_make) {
      printf("Making files...\n");
      fflush(stdout);
    }

    /* Bandwidth testing requires bandwidth_test0 through 9.
     * If they don't exist, make them.
     */
#ifdef HAS_SRANDOMDEV
    srandomdev();
#else
    srandom(time(0));
#endif

    size_t size = BANDWIDTH_DEFAULT_SIZE;
    char *buf = (char *)malloc(size);
    memset(buf,'E',size);
    if(argc>0) size=atoi(argv[0]);

    s3_debug = 1;			// print retries

    char base[1024];
    char randp[1024];
    sprintf(base,"%s.%d",BANDWIDTH_PREFIX,size);
    sprintf(randp,"%s.%d.%d",BANDWIDTH_PREFIX,size,random() % 1000);


    if(opt_make){
	strcpy(randp,base);		// just write to this one
    }

    /* Measure the write bandwidth */
    aftimer twrite;
    twrite.start();
    for(int i=0;i<BANDWIDTH_COUNT;i++){
	char key[1024];
	sprintf(key,"%s.%d",randp,i);
	object_put(opt_bucket,key,buf,size,0);
	write_retry += s3_request_retry_count;
	write_err   += s3_object_put_retry_count;
    }
    twrite.stop();


    if(opt_make){
	printf("done\n");
	exit(0);
    }
    /* Delete the writes */
    for(int i=0;i<BANDWIDTH_COUNT ;i++){
	char key[1024];
	sprintf(key,"%s.%d",randp,i);
	if(object_rm(opt_bucket,key)){
	    err(1,"object_rm failed\n");
	}
    }
    

    aftimer tread;
    /* Now measure the read bandwidth */
    tread.start();
    for(int i=0;i<BANDWIDTH_COUNT ;i++){
	char key[1024];
	sprintf(key,"%s.%d",base,i);
	response_buffer *r;
	object_get(opt_bucket,key,0);
	read_retry += s3_request_retry_count;
    }
    tread.stop();

    time_t t = twrite.tstart();
    struct tm *tm = gmtime(&t);
    char tbuf[64];
    strftime(tbuf,sizeof(tbuf),"%F %T",tm);

    printf("%s\t%d\t%10.2f\t%10.2f\t%10.2f\t%10.2f\t%d\t%d\t%d\n",
	   tbuf,
	   BANDWIDTH_COUNT*size,
	   twrite.elapsed_seconds()/BANDWIDTH_COUNT,
	   (BANDWIDTH_COUNT*size)/twrite.elapsed_seconds(),
	   tread.elapsed_seconds()/BANDWIDTH_COUNT,
	   (BANDWIDTH_COUNT*size)/tread.elapsed_seconds(),
	   write_retry,write_err,read_retry);
    
    exit(0);
}


/****************************************************************/


int main(int argc,char **argv)
{
    int bflag, ch;

    bflag = 0;
    opt_bucket = getenv("S3_DEFAULT_BUCKET");
    while ((ch = getopt(argc, argv, "b:dvh?f:m")) != -1) {
	switch (ch) {
	case 'd': s3_debug++;break;
	case 'b': opt_bucket = optarg;break;
	case 'f': opt_flag = atoi(optarg);break;
	case 'm': opt_meta = 1;break;
	case 'v':
	case 'h':
	case '?':
	default:
	    usage();
	}
    }
    argc -= optind;
    argv += optind;

    if(argc<1){
	usage();
    }

    if(getenv("S3_DEBUG")){
	s3_debug = atoi(getenv("S3_DEBUG"));
#ifdef HAS_ERR_SET_EXIT
	err_set_exit(s3_audit);
#endif
    }

    aws_access_key_id     = getenv("AWS_ACCESS_KEY_ID");
    aws_secret_access_key = getenv("AWS_SECRET_ACCESS_KEY");
    if(!aws_access_key_id) fprintf(stderr,"s3: AWS_ACCESS_KEY_ID not defined\n");
    if(!aws_secret_access_key) fprintf(stderr,"s3: AWS_SECRET_ACCESS_KEY not defined\n");
    if(!aws_access_key_id || !aws_secret_access_key) return -1; /* can't open */
    if(!opt_bucket) {
	fprintf(stderr,"No bucket. Please setenv S3_DEFAULT_BUCKET or specify -b option.\n");
	exit(1);
    }


    char *cmd = argv[0];

    if(!strcmp(cmd,"ls") || !strcmp(cmd,"dir")){
	const char *prefix = argc>1 ? argv[1] : "";
	s3_ls(stdout,prefix,0);exit(0);
    }
    if(!strcmp(cmd,"df")) {s3_df();exit(0);}
    if(!strcmp(cmd,"cat")) {s3_cat(*argv);exit(0);}
    if(!strcmp(cmd,"rm")) {s3_rm(argc,argv);exit(0);}
    if(!strcmp(cmd,"cp") || !strcmp(cmd,"put") || !strcmp(cmd,"send")) {s3_cp(*argv,argv[2]);exit(0);}
    if(!strcmp(cmd,"mkdir")) {s3_mkdir(argc,argv);exit(0);}
    if(!strcmp(cmd,"rmdir")) {s3_rmdir(*argv);exit(0);}
    if(!strcmp(cmd,"regress")) {regress();exit(0);}
    if(!strcmp(cmd,"delp")) {s3_delp(argc,argv);exit(0);}
    if(!strcmp(cmd,"bandwidth")) {
	opt_bucket = "bandwidth";
	s3_bandwidth(argc,argv);exit(0);
    }
    usage();
    exit(0);
}
#else
int main(int argc,char **argv)
{
    fprintf(stderr,"S3 is not compiled in.\n");
    exit(0);
}
#endif
