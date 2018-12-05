
/* 测试利用occi接口连接数据库 author:liuqian  date:2017-2-7  */

#include <iostream>
#include <occi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <libxml/parser.h>  
#include <libxml/tree.h>  
#include <sys/time.h>

using namespace std;
using namespace oracle::occi;


#define  ARRAYSIZENUM  64  

void  getSubCurNode(int index ,char *sqlID,xmlDocPtr doc, xmlNodePtr cur);

typedef struct sqlmsg
{
	int argCnt;
	char sqlArray[256];
	
}_sqlmsg;

_sqlmsg  tepsql[ARRAYSIZENUM];


long getCurrentMills() {
    struct timeval curr;
    gettimeofday( &curr, NULL );
    return  1000 * curr.tv_sec + curr.tv_usec/1000;  // unit is  millisecond
}


class occiproctest
{
	private:
		oracle::occi::Environment *env;
		oracle::occi::Connection *conn;
		time_t lastTime;
		int      lastCount;
		int      failedNum;
	public:
		int      minTime;
		int      maxTime;
		int      averageTime;
	public:
	occiproctest (std::string user, std::string passwd, std::string db) throw (oracle::occi::SQLException)
	{
		env = oracle::occi::Environment::createEnvironment(oracle::occi::Environment::DEFAULT);
		conn = env->createConnection (user,passwd,db);
		if(conn == NULL )
		{
			printf(" conn  db  is  success !");
		}
		lastTime = time(NULL);
		lastCount = 0;
		failedNum = 0;
		minTime = 99999999;
		maxTime = 0;
		averageTime = 0;
	}
	~occiproctest () throw (oracle::occi::SQLException)
	{
		env->terminateConnection (conn);  //断开数据库连接
		oracle::occi::Environment::terminateEnvironment (env); //终止环境变量
	}

	//获取文件大小
	/*long get_file_size(char* filename)
	{
		long length = 0;
		FILE *fp = NULL;
		fp = fopen(filename, "rb");
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			length = ftell(fp);
		}
		if (fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
		return length;
	}*/


	//输出: 时间,id,sql,cost time ,min time,max time ,average,time
	void WriteLogFile(char* exesqlId)
	{
	
		char sSrecord[4*ARRAYSIZENUM];
		char tem[ARRAYSIZENUM];
		
		time_t timer;	//time_t就是long int 类型
		struct tm *tblock;

		timer = time(NULL);
		tblock = localtime(&timer);
		
		memset(tem,'\0',sizeof(tem));
		sprintf(tem,"%d%.2d%.2d%.2d%.2d%.2d",1900+tblock->tm_year,1+tblock->tm_mon,tblock->tm_mday,tblock->tm_hour, tblock->tm_min, tblock->tm_sec);

		memset(sSrecord,0,sizeof(sSrecord));
		sprintf(sSrecord ,"%s,%s,%d,%d,%d\n",tem,exesqlId,minTime,maxTime,averageTime);

		FILE *fp;
		fp = fopen("oratestor.log","ab+");
		fwrite(sSrecord ,strlen(sSrecord),1,fp);	
		fclose(fp);
		
	}
	void ExecuteSQL(char *sqlArray)
	{
		ResultSet    *rs;
		Statement  *stmt;
	   	string sql,sStrname,sSubsid,sSex;
		char tepBuf[256];

		stmt =conn->createStatement();//创建Statement对象
		stmt->setSQL(sqlArray); //准备sql语句
	
		try
		{
			rs = stmt->executeQuery();//执行sql语句，返回结果集


			while(rs->next())//取数据
			{
				sSubsid = rs->getString(1);
				sStrname = rs->getString(3);
				sSex = rs->getString(9);
				//cout << sSubsid <<"\t"<<sStrname<<"\t"<<sSex<<endl ;	
			}
		}
		  catch(oracle::occi::SQLException ex) //异常处理
	        {
	                cout << " Error Number "<< ex.getErrorCode() << endl;
	                cout << ex.getMessage() << endl;
	        }
		  conn->terminateStatement(stmt);  //终止Statement对象
	}


};

/*class  LIBXMLParseDocdument
{

	private:
		xmlDocPtr doc;  
		xmlNodePtr cur;
		
	public:
	LIBXMLParseDocdument()
	{
    		xmlKeepBlanksDefault(0);  
	}
	~LIBXMLParseDocdument()
	{
		//释放文档指针
		xmlCleanupParser();  
		xmlMemoryDump();  
	}
	void parsedoc(char * sqid,char *sqlconfig);
	void  getSubCurNode(char *sqlId ,xmlDocPtr doc, xmlNodePtr cur);
	
};
void LIBXMLParseDocdument::parsedoc(char *sqlId,char *sqlconfig)
{
    	//解析文档  
    	doc = xmlParseFile(sqlconfig);  
	if(NULL == doc)
	{
	   fprintf(stderr,"Document not parsed successfully.\n");  
      	   return;  
	}
	
	//获取根结点  
	cur = xmlDocGetRootElement(doc);  
	//输出根结点信息  
	//printf("root node=%s\n", cur->name);  
	if (cur == NULL) 
	{  
	    fprintf(stderr,"empty document\n");  
	    xmlFreeDoc(doc);  
	    return;  
	}  
	//获取子节点
	getSubCurNode(sqlId,doc,cur);
	xmlFreeDoc(doc);  
	return ;		
}

void LIBXMLParseDocdument:: getSubCurNode(char *sqlId ,xmlDocPtr doc, xmlNodePtr cur)
{
	
}
*/

//解析xml文件  
void parseDoc(int index,char *sqlId,char *sqlconfig)
{  
  
    xmlDocPtr doc;  
    xmlNodePtr cur;  

    xmlKeepBlanksDefault(0);  
    //解析文档  
    doc = xmlParseFile(sqlconfig);  
  
    if (doc == NULL )
   {  
        fprintf(stderr,"Document not parsed successfully.\n");  
        return;  
    }  
    //获取根结点  
    cur = xmlDocGetRootElement(doc);  
    //输出根结点信息  
   //printf("root node=%s\n", cur->name);  
   if (cur == NULL) 
   {  
        fprintf(stderr,"empty document\n");  
        xmlFreeDoc(doc);  
        return;  
    }  

   //获取子节点
   getSubCurNode(index,sqlId,doc,cur);
   xmlFreeDoc(doc);  
   return ;
   
}  

//获取子节点元素内容和节点属性值
void  getSubCurNode(int index, char *sqlId , xmlDocPtr doc, xmlNodePtr cur)
{
   xmlChar* key;  
   xmlChar* attr_value;

   if(cur != NULL)
   {
   	cur = cur->xmlChildrenNode;//get sub node
   	while(cur != NULL)
	   {
	   	//节点的类型XML_ELEMENT_NODE时才获取属性  
	   	if(cur->type == XML_ELEMENT_NODE)
	   	{
	                attr_value = xmlGetProp(cur, (const xmlChar *)"sqlid");  
			if(strcmp(sqlId,(const char *)attr_value) == 0)
			{
				memset(tepsql[index].sqlArray,'\0' ,sizeof(tepsql[index].sqlArray));
				//获取节点内容
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1); 
				strcpy(tepsql[index].sqlArray,(const char *)key);
				xmlFree(key);  
			}
	                //if(attr_value != NULL)  
	                //    printf("data: %s",attr_value);  
	                xmlFree(attr_value);     //释放资源
	                //printf("\n");  	
	   	}
		getSubCurNode(index,sqlId,doc,cur);
		cur = cur->next;
	   }
	return ;
   	}
   else
   {
	fprintf(stderr, "ERROR: Null Node!");  
        return;   
   }

}


void helpManual(void)
{
	printf("Argument parsing as follows:\n\n");
	printf("   -u  \tUsage:Connect to the database user name .  ex:[misc201]\n");
	printf("   -p  \tUsage:Connect to the database user password .  ex:[misc201]\n");
	printf("   -s  \tUsage:Connect to the database  character string .  ex:[10g220]\n");
	printf("   -n\tUsage:The total number of executing SQL statements request .  ex:[100]\n");
	printf("   -i \tUsage: Input sqlid string has a comma.  ex:[3,4,5]\n");
	printf("   -f \tUsage:According to the sqlid  in the configuration files name.  ex:[sqlconfig.xml]\n\n");
	
}
int main(int argc , char** argv)
{

	int ch;
	char sConnStr[ARRAYSIZENUM] = {'\0'};
	char sUser[ARRAYSIZENUM] = {'\0'};
	char sPasswd[ARRAYSIZENUM] = {'\0'};
	char stId[ARRAYSIZENUM] ={'\0'};
	char ssqlCfgfName[ARRAYSIZENUM] ={'\0'};
	int  iReqSum = 0;
	
	while((ch = getopt(argc,argv,"h::u:p:s:n:c:o:i:f:")) != -1)
	{
		switch(ch)
		{
		case 'h':
			helpManual();
			return 0;
		case 'u':
			//printf("connect  oracle  db  User:%s\n",optarg);
			strcpy(sUser,optarg);
			break ;
		case 'p':
			//printf("connect  oracle  db  Pwd:%s\n",optarg);
			strcpy(sPasswd,optarg);
			break;
		case 's':
			//printf("connect  oracle  db  Constring:%s\n",optarg);
			strcpy(sConnStr,optarg);
			break;
		case 'n':
			//printf("output  paramter : %d %s %d %c\n",optind,optarg,ch,ch);
			iReqSum = atoi(optarg) ;
			//printf("excute request  sum:%d\n",iReqSum);
			break;
		/*case 'c':
			printf("output  Concurrent  num:%s\n",optarg);
			break;
		*/	
		case 'i':
			//printf("output  sql  id  string:%s\n",optarg);
			//argIndex = optind;
			strcpy(stId,optarg);
			break;
		case 'f':
			//printf("output sqlconfigxml file:%s %d %c\n",optarg,ch,ch);
			strcpy(ssqlCfgfName,optarg);
			break;
		default:
			printf("other option:%c\n",ch);
		}
	
	}

	//printf("oracle User:%s  oracle Pwd:%s oracle Constring : %s\n",sUser,sPasswd,sConnStr);

	LIBXML_TEST_VERSION     //使用一个宏来测试当前libxml和使用的库文件是否匹配  

	char *delim =",";  //sql id串以逗号分隔
	char *p = NULL;
	char  strVec[32][32] ;
	
	memset(strVec,'\0',sizeof(strVec));
	
	int nCnt = 0 ;

	if( 0 == strlen(stId) )
	{
		printf("The number of requests you entered is invalid \n");
	}
	else if( NULL!= (p = strtok(stId,delim)))
	{
		strcpy(strVec[nCnt],p);
		//printf(" is  p ->  value :%s   strVec: %s\n",p,strVec[nCnt]);
		nCnt++;
		//printf("strtok:%d----%s---\n",nCnt-1,strVec[nCnt-1]);
		 while((p = strtok(NULL, delim)))
		 {
		 	strcpy(strVec[nCnt],p);
			//printf("strtok:%d--%s---\n",nCnt,strVec[nCnt]);
			nCnt++;
		 }
	}
	else 
	{
		printf("input  value  valied!\n");
	}
	occiproctest  octest(sUser,sPasswd,sConnStr);   //创建对象

	memset(tepsql,'\0',sizeof(_sqlmsg)*ARRAYSIZENUM);
	
	for(int sIndex= 0; sIndex < nCnt ;sIndex++ )
	{
		parseDoc(sIndex,strVec[sIndex],ssqlCfgfName); //根据id串解析获取sql执行语句
		
	}

	long endtime = 0;
	long starttime = 0;
	long npersCosttime = 0;
	long ntotalCost = 0;

	//文件说明
	FILE *fp;
	char usageHead[2*ARRAYSIZENUM];
	char usageTail[2*ARRAYSIZENUM];

	memset(usageHead,0,sizeof(usageHead));
	memset(usageTail,0,sizeof(usageTail));
	strcpy(usageHead,"\n    date,sqlid, minTime, maxTime, aveTime\n------------------------------------\n");
	//strcpy(usageTail,"total time:");
	printf("###################\n");
	fp = fopen("oratestor.log","ab+");
	if(fp == NULL)
	{
		printf("open filed !\n");
		exit(0);
	}
	fwrite(usageHead,strlen(usageHead),1,fp);
	fclose(fp);
	
	//循环执行-n 输入的参数
	for(int j =0 ; j < nCnt ; j++)
	{
		//判断sqid 是否存在于 配置文件中
		if( 0 == strlen(tepsql[j].sqlArray)) 
		{
			printf("The input sqlid in the configuration file does not exist!\n");
			continue;
		}
		printf("start  runing  [id:%s]\n",strVec[j]);
		for(int k = 0; k < iReqSum ; k++)
		{	
			
			//printf("The current this number %d  \t",k+1);
			starttime= getCurrentMills();
			octest.ExecuteSQL(tepsql[j].sqlArray);
			endtime = getCurrentMills();

			npersCosttime = endtime - starttime ;
			ntotalCost += npersCosttime ;
			
			if(npersCosttime < octest.minTime)octest.minTime = npersCosttime ;
			if(npersCosttime > octest.maxTime)octest.maxTime = npersCosttime ;
			
			octest.averageTime = ntotalCost/(k+1);

			printf("excute sql costime(ms):%ld ,min=%ld ,max=%ld ,average=%ld \n",npersCosttime,octest.minTime ,octest.maxTime ,octest.averageTime );

			octest.WriteLogFile(strVec[j]);
			
		}
		
		sprintf(usageTail , "excute sqid:%s--->total time:%lds\n",strVec[j],ntotalCost/1000);
		fp = fopen("oratestor.log","ab+");
		fwrite(usageTail,strlen(usageTail),1,fp);
		fclose(fp);
		memset(usageTail,'\0',sizeof(usageTail));
		
	}
	
	//parseDoc(strVec[0],ssqlCfgfName); //根据输入sqlid查找xml文件对应sql执行语句
	//释放文档指针
	xmlCleanupParser();  
	xmlMemoryDump();  
	
	return  0;
}


