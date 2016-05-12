/****************************************************************************/
//All the codes in the software are subject to the GNU General Public License("GPL") or 
//GNU Lesser General Public License("LGPL").
//Please see the GNU and LPGL Web sites to view the terms of each license.

/* Purpose :                                                                */
/*   1. to provide re-configurability via INI file                          */
/* Remarks :                                                                */
/* History :                                                                */
/* DDMMYYYY Author Description                                              */
/* ======================================================================== */
// 20080915 Zhengyi interface, saving time, add '\t'
/****************************************************************************/

#include "stdafx.h"
#include "mtndefin.h"// header file to define the return value, // 20080915

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>

//#include "stdafx.h"
#include "mtnconfg.h" 

//#define __STUB__
//#define LOOP_TEST
#define CFG_MAX_STRLEN_DEBUG_CHAR 512
static char strDebugMessage_MtnConfig[CFG_MAX_STRLEN_DEBUG_CHAR];

#define CFG_MAX_STRLEN_FILENAME   512
static FILE *fp_config = NULL;
static char cfg_file_pathname[CFG_MAX_STRLEN_FILENAME];
static MTN_CFG_PRF_BLK cfg_table[CFG_MAX_NUM_BLOCKS];                 /* statically allocated table */
static short cfg_count=0;

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfg_GetConfigInfo(MTN_CFG_PRF_BLK *pBlk, short *pcount)      */
/* Purpose :                                                                */
/*   1. to get configuration file total blocks & its start line             */
/* Arguments:                                                               */
/*   1. MTN_CFG_PRF_BLK *pBlk -- to store the block info                    */
/*   2. short *pcount -- to store the total block number                    */
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* ------------------------------------------------------------------------ */
short mtn_cfg_GetConfigInfo(MTN_CFG_PRF_BLK *pBlk, short *pcount)
{
    int ii;
    for(ii=0;ii<cfg_count;ii++)
    {
        pBlk[ii] = cfg_table[ii];
    }
    *pcount = cfg_count;
    return MTN_API_OK_ZERO;
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfg_OpenConfigFile(char *p_config_pathname)                      */
/* Purpose :                                                                */
/*	 1. to open a configuration file and build up its configuration profile */
/* Arguments:                                                               */
/*	 1. p_config_pathname - the DOS path of the configuration file			*/
/* Modified:                                                                */
/*	 1. cfg_table content will be updated									*/
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* Remark :                                                                 */
/*	 1. size of table not handled, assumed sufficient						*/
/*   2. tightly related to the structure and intention of the CFG_PRF_BLK   */
/* ------------------------------------------------------------------------ */
short mtn_cfg_OpenConfigFile(char *p_config_pathname)
{
    char buffer[BUFSIZ+1];
    short status;
    long start_pos, file_size;
    short line_count;
    char state;
    unsigned int ii;
    short start_loc, end_loc;
    auto struct stat stat_buf;

    cfg_count = 0;
    for (ii=0; ii<128; ii++)
    {
        cfg_table[ii].prf_blk_name[0] = '\0';
        cfg_table[ii].start_file_pos = 0;
        cfg_table[ii].end_file_pos = 0;
        cfg_table[ii].num_of_lines = 0;
    }

    status = OPENERR;
    fp_config = NULL;
    fopen_s(&fp_config, p_config_pathname, "r");
    if (fp_config == NULL)
    {
		sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Open file %s Error!\n", 
			__FILE__, __LINE__, p_config_pathname);
        return MTN_API_ERROR_OPEN_FILE;
    }

    //  added to keep a copy of current config file name.
    sprintf_s(cfg_file_pathname, __MAX_FILE_NAME__, "%s", p_config_pathname);

    if (fp_config != NULL)
    {
        // fprintf(stderr, "Open file %s success!\n", p_config_pathname);

        start_pos = 0;
        line_count = 1;
        state = '[';
        cfg_count = 0;
        if ( stat (p_config_pathname, &stat_buf) != -1)
            file_size = stat_buf.st_size;
        else
        {
            /* need to handle this situation, but embedded system, how? */
        }

        while (!feof(fp_config))
        {
            start_pos = ftell(fp_config);

            if (fgets(buffer, BUFSIZ, fp_config) != NULL)
            {
                /* for every new line read, look for the character [ that   */
                /* indicate start of a block and then look for ] that is    */
                /* the closing bracket for the configuraruion block name    */
                state = '[';
                for (ii = 0; ii < strlen(buffer); ii++)
                {
                    switch (state)
                    {
                        case '[' :
                            if (buffer[ii] == state)
                            {
                                state = ']';
                                start_loc = ii+1;
                            }
                            break;
                        case ']' :
                            if (buffer[ii] == state)
                            {
                                end_loc = ii-1;
                                state = 'X';
                            }
                            break;

                        default:
                        break;
                    }
                }

                /* 3 scenarios:                                             */
                /* 1. if block name can't be found, state will stay at [    */
                /*    which means this line carries the content under the   */
                /*    last block, remember the no of lines for this block   */
                /* 2. somehow, just got the starting [ but not the ], error */
                /*    just ignore                                           */
                /* 3. if state end at 'X', got one block                    */
                if (state == '[')
                    line_count ++;
                else if (state == ']')
                {
                    /* error case, reset */
                    line_count ++;
                }
                else if (state == 'X')
                {
                    /* OK, got one config block */
                    strncpy_s(&cfg_table[cfg_count].prf_blk_name[0] , MTN_API_MAX_STRLEN_FEATURENAME,
                        &buffer[start_loc], (end_loc-start_loc+1));
                    cfg_table[cfg_count].prf_blk_name[(end_loc-start_loc+1)] = '\0';
                    cfg_table[cfg_count].start_file_pos = start_pos;
                    cfg_table[cfg_count].end_file_pos = file_size;
                    cfg_table[cfg_count].num_of_lines = line_count;
                    if (cfg_count > 0)
                    {
                        cfg_table[cfg_count-1].end_file_pos = start_pos;
                        cfg_table[cfg_count-1].num_of_lines = line_count;
                    }
                    cfg_count ++;
                    line_count = 1;
                    status = OPENOK;
                }
            }

            /* bug fix for 1 block only config file, ref 200302A */
            if (feof(fp_config))
            {
                /* if this is the end of file and state still stay at [  */
                /* that mean we have now got the parameters for the last */
                /* block, update the line_count and end_file_pos now     */
                if (state == '[' && cfg_count > 0)
                {
                    cfg_table[cfg_count-1].end_file_pos = file_size;
                    cfg_table[cfg_count-1].num_of_lines = line_count;
                }
            }
            /* end of 200302A */
        }
    }
    return (status);
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgReadConfigString(char *p_blockname, char *p_config,          */
/*		 char *p_parameter) 												*/
/* Purpose :                                                                */
/*	 1. to read the value assigned to a particular configuration string 	*/
/*		<p_config> that belong to block <p_blockname> into <p_parameter>	*/
/* Arguments:                                                               */
/*	 1. p_blockname - the configuration block								*/
/*	 2. p_config	- the configuration string in the configuration block	*/
/*	 3. p_parameter - the value 											*/
/* Modified:                                                                */
/*	 1. string pointed to by p_parameter if configuration is found			*/
/* Return:                                                                  */
/*	 OPENERR - not OK, 0 - OK 													*/
/* Remark :                                                                 */
/*   1. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_ReadConfigString(char *p_blockname, char *p_config,
    char *p_parameter)
{
    char buffer[BUFSIZ+1];
    char *ptr;
    short status;
    short ii, ll, blkno;

    status = OPENERR;
    blkno = -1;

	/* search through the cfg_table for block identified by <p_blockname>	*/
    for (ii=0; ii<cfg_count; ii++)
    {
        if (strcmp(p_blockname, cfg_table[ii].prf_blk_name) == 0)
        {
            blkno = ii;
            break; // ii = cfg_count + 1;, once find the first, donot continue to search and waist time, 20080915
        }
    }

    status = OPENERR;
	if (blkno != -1)
    {
        status = OPENERR;
		/* the desired block found, now, move the file pointer to the		*/
		/* starting line for that block 									*/
        fseek(fp_config, cfg_table[blkno].start_file_pos, 0);

		/* try to locate the configuration string within the number of line */
		/* of the block, and then read it value 							*/
        for (ll=0; ll<cfg_table[blkno].num_of_lines && status == OPENERR; ll++)
        {
            if (fgets(buffer, BUFSIZ, fp_config) != NULL)
            {
                //  Added 30Oct2003
                ptr = &buffer[0];
                while(ptr[0] == ' ') // skip the space at the beginning
                    ptr++;

                if (*ptr != '#' && *ptr != ';') // if the line starts with '#' or ';', the line is a comment line
                {
					/* this is not a remark line, locate the desired string */
					/* if string found, check for equal sign then read the	*/
					/* parameter											*/
                    ptr = strstr(ptr, p_config);
                    if (ptr != NULL)
                    {
                        ptr = ptr + strlen(p_config);

                        //  Added 5Jan2004 to exactly match the config string
                        //  seperation string can only be one of followings 20080915
						// ' ': space, 
						// '=': equality,
						// '\t': Tab
                        if (ptr[0] != ' ' && ptr[0] != '=' && ptr[0] != '\t')  // 20080915
                        {
                            continue;
                        }

						/* look for equal sign */
						ptr = strpbrk(ptr, "=");
                        if (ptr != NULL)
                        {
                            ptr++;
                            while (*ptr == ' ')
                                ptr++;

                            if (*ptr == '\"') // ignore the string starting and ending: "
                            {
                                ptr++;
                                ii=0;
                                while (*(ptr+ii) != '\"')
                                    ii++;
                                *(ptr+ii) = 0;
                                strcpy(p_parameter, ptr);
                                status = READOK;
                            }
                            //@a, add one more case. // can read a block structure
                            else if (*ptr == '<')
                            {
                                ptr++;
                                ii=0;
                                while (*(ptr+ii) != '>')
                                    ii++;
                                *(ptr+ii) = 0;
                                strcpy(p_parameter, ptr);
                                status = READOK;
                            }
                            else
                            {
                                ii=0;
                                while ((*(ptr+ii) != ' ')  &&
                                       (*(ptr+ii) != '#')  &&
                                       (*(ptr+ii) != '\n') &&
                                       (*(ptr+ii) != '\r') &&
									   (*(ptr+ii) != '\t') &&  // 20080915
                                       (*(ptr+ii) != 0) )
                                    ii++;
								*(ptr+ii) = 0;	/* end the string */
                                strcpy(p_parameter, ptr);
                                status = READOK;
							}	/* is it string or number */
						}	/* if equal sign found */
					}	/* if string found */
				}	/* if not remark line */
            }   /* if fgets ... */
        }   /* for loop */
    }
    return (status);
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgReadConfigIntValue(char *p_blockname, char *p_config,        */
/*		 int *p_parameter)													*/
/* Purpose :                                                                */
/*	 1. to read an integer value assigned to a particular configuration 	*/
/*		string <p_config> that belongs to a configuration block 			*/
/*		<p_blockname> into a variable pointed to by <p_parameter>			*/
/* Arguments:                                                               */
/*	 1. p_blockname - the configuration block								*/
/*	 2. p_config	- the configuration string in the configuration block	*/
/*	 3. p_parameter - the integer value 									*/
/* Modified:                                                                */
/*	 1. number pointed to by p_parameter if configuration is found			*/
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* Remark :                                                                 */
/*   1. calls mtn_cfgReadConfigString                                          */
/*   2. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_ReadConfigIntValue(char *p_blockname, char *p_config,
	int *p_parameter)
{
    char string[128];
    short stat;

    stat = mtn_cfg_ReadConfigString(p_blockname, p_config, &string[0]);
    if (stat != 0)
    {
        /* not very safe */
        *p_parameter = atoi(string);
        stat = 1;
    }

    return stat;
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgReadConfigRealValue(char *p_blockname, char *p_config,       */
/*       float *p_parameter)                                                */
/* Purpose :                                                                */
/*	 1. to read an integer value assigned to a particular configuration 	*/
/*		string <p_config> that belongs to a configuration block 			*/
/*		<p_blockname> into a variable pointed to by <p_parameter>			*/
/* Arguments:                                                               */
/*	 1. p_blockname - the configuration block								*/
/*	 2. p_config	- the configuration string in the configuration block	*/
/*   3. p_parameter - the floating point number                             */
/* Modified:                                                                */
/*	 1. number pointed to by p_parameter if configuration is found			*/
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* Remark :                                                                 */
/*   1. calls mtn_cfgReadConfigString                                          */
/*   2. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_ReadConfigRealValue(char *p_blockname, char *p_config,
    float *p_parameter)
{
    char string[128];
    short stat;

    stat = mtn_cfg_ReadConfigString(p_blockname, p_config, &string[0]);
    if (stat != 0)
    {
        /* not very safe */
        *p_parameter = (float)atof(string);
        stat = 1;
    }

    return stat;
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgReadConfigDoublePrecisionValue(char *p_blockname, char *p_config,   */
/*       double *p_parameter)                                                */
/* Purpose :                                                                */
/*	 1. to read an integer value assigned to a particular configuration 	*/
/*		string <p_config> that belongs to a configuration block 			*/
/*		<p_blockname> into a variable pointed to by <p_parameter>			*/
/* Arguments:                                                               */
/*	 1. p_blockname - the configuration block								*/
/*	 2. p_config	- the configuration string in the configuration block	*/
/*   3. p_parameter - the floating point number                             */
/* Modified:                                                                */
/*	 1. number pointed to by p_parameter if configuration is found			*/
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* Remark :                                                                 */
/*   1. calls mtn_cfgReadConfigString                                          */
/*   2. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_ReadConfigRealValue(char *p_blockname, char *p_config,
    double *p_parameter)
{
    char string[128];
    short stat;

    stat = mtn_cfg_ReadConfigString(p_blockname, p_config, &string[0]);
    if (stat != 0)
    {
        /* not very safe */
        *p_parameter = atof(string);
        stat = 1;
    }

    return stat;
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgCloseConfigFile(void)                                        */
/* Purpose :                                                                */
/*	 1. just close the file pointer 										*/
/* Arguments:                                                               */
/*	 NIL																	*/
/* Return:                                                                  */
/*	 always OK																*/
/* Remark :                                                                 */
/*   2. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_CloseConfigFile(void)
{
    short status = READOK;

    fclose(fp_config);

    return (status);
}

// Return:
// 0 - not OK, 1 - OK
// example:
// input -- p_string = " 1, 2,3,  4,5 $W"
// input -- p_string = " 1.1, 2,3,  4,5.0 $F"
short mtn_cfg_Count_String(char *p_string, int *pNum, char* delims)
{
    char    *buffer;
    char    *ptr;

    if (p_string == NULL)
        return MTN_API_ERROR_COUNT_NULL_STRING;
    buffer = strdup(p_string);  // duplicate string
    if (buffer == NULL)
        return MTN_API_ERROR_MEMORY;

    *pNum = 0;
    ptr = strtok(buffer, delims);
    while(ptr != NULL)
    {
        (*pNum) ++;
        // fprintf(stderr,"%d: %s\n", *pNum, ptr);
        ptr = strtok(NULL, delims);
    }

    free(buffer);
    return MTN_API_OK_ZERO;
}


/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgmodify_string(FILE *fpSrc, long posn, char *p_config,        */
/*                           char *p_parameter)                             */
/* Purpose :                                                                */
/*   1. to modify parameter of the specified config string in the current   */
/*      file.                                                               */
/* Arguments:                                                               */
/*   1. fpSrc       - Current configuration file pointer                    */
/*   2. posn        - the file position where the config string is          */
/*                    located                                               */
/*   3. p_config    - the configuration string in the configuration block   */
/*   4. p_parameter - the new parameter                                     */
/* Return:                                                                  */
/*   0 - not OK, 1 - OK                                                     */
/* Remark :                                                                 */
/*   1. This function is called within mtn_cfgPutConfigString().               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_modify_string(FILE *fpSrc, long posn, char *p_config, char *p_parameter)
{
    char buffer[BUFSIZ+1], filename[12];
    long curr_posn;
    int  iNum;
    FILE *fpDest;

    // open a temporary file
    fpDest = NULL;
    tmpnam(filename);
    fopen_s(&fpDest, filename, "w");
    if(fpSrc==NULL || fpDest == NULL)
    {
        sprintf_s(strDebugMessage_MtnConfig, 512, "[%s %d]:Invalid file pointer!\n", __FILE__, __LINE__);
        return MTN_API_ERR_FILE_PTR;
    }

    // Point to the start position of the file
    fseek(fpSrc, 0, SEEK_SET);
    while (!feof(fpSrc))
    {
        curr_posn = ftell(fpSrc);
        if(fgets(buffer, BUFSIZ, fpSrc) != NULL)
        {
            if (curr_posn!=posn)
                fprintf(fpDest, "%s", buffer);
            else
            {
                mtn_cfg_Count_String(p_parameter, &iNum, " ,");
                if (iNum > 1)
                    fprintf(fpDest, "%s = <%s>\n", p_config, p_parameter);
                else
                    fprintf(fpDest, "%s = %s\n", p_config, p_parameter);
            }
        }
    }
    fclose(fpSrc);
    fclose(fpDest);
    remove(cfg_file_pathname); // deletes a file from memory
    if (rename(filename, cfg_file_pathname) !=0)
    {
        sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Rename failed!\n", __FILE__, __LINE__);
        return MTN_API_ERR_RENAME_FILE;
    }

    // Open the New Configuration file
    mtn_cfg_OpenConfigFile(cfg_file_pathname);
    return MTN_API_OK_ZERO;
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfginsert_string(FILE *fpSrc, long posn, char *p_config,        */
/*                           char *p_parameter)                             */
/* Purpose :                                                                */
/*   1. to insert a config string & its parameter in the current            */
/*      file.                                                               */
/* Arguments:                                                               */
/*   1. fpSrc       - Current configuration file pointer                    */
/*   2. posn        - the file position where the config string is          */
/*                    to be located                                         */
/*   3. p_config    - the new configuration string in the configuration block */
/*   4. p_parameter - the new parameter                                     */
/* Return:                                                                  */
/*   0 - not OK, 1 - OK                                                     */
/* Remark :                                                                 */
/*   1. This function is called within mtn_cfgPutConfigString().               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_insert_string(FILE *fpSrc, long posn, char *p_config, char *p_parameter)
{
    char buffer[BUFSIZ+1], filename[12];
    long curr_posn;
    int  iNum;
    FILE *fpDest;

    // open a temporary file
    fpDest = NULL;
    tmpnam(filename);
    fopen_s(&fpDest, filename, "w");
    if(fpSrc==NULL || fpDest == NULL)
    {
        sprintf_s(strDebugMessage_MtnConfig, 512, "[%s %d]:Invalid file pointer!\n", __FILE__, __LINE__);
        return MTN_API_ERR_FILE_PTR;
    }

    // Get the number of value in the string
    mtn_cfg_Count_String(p_parameter, &iNum, " ,");

    // Point to the start position of the file
    fseek(fpSrc, 0, SEEK_SET);
    while (!feof(fpSrc))
    {
        curr_posn = ftell(fpSrc);
        if(fgets(buffer, BUFSIZ, fpSrc) != NULL)
        {
            if (curr_posn!=posn)
            {
                fprintf(fpDest, "%s", buffer);
            }
            else
            {
                if (iNum > 1)
                    fprintf(fpDest, "%s = <%s>\n", p_config, p_parameter);
                else
                    fprintf(fpDest, "%s = %s\n", p_config, p_parameter);

                fprintf(fpDest, "%s", buffer);
            }
        }
        // Handling for Block located at the last line
        if (feof(fpSrc))
        {
            curr_posn = ftell(fpSrc);
            if (curr_posn==posn)
            {
                if (iNum > 1)
                    fprintf(fpDest, "%s = <%s>\n", p_config, p_parameter);
                else
                    fprintf(fpDest, "%s = %s\n", p_config, p_parameter);
            }
        }
    }
    fclose(fpSrc);
    fclose(fpDest);
    remove(cfg_file_pathname);
    if (rename(filename, cfg_file_pathname) !=0)
    {
        sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Rename failed!\n", __FILE__, __LINE__);
        return MTN_API_ERR_RENAME_FILE;
    }

    // Open the New Configuration file
    mtn_cfg_OpenConfigFile(cfg_file_pathname);
    return MTN_API_OK_ZERO;
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgPutConfigString(char *p_blockname, char *p_config,          */
/*		 char *p_parameter) 												*/
/* Purpose :                                                                */
/*   1. If <p_blockname> exists in the current configuration file, then     */
/*      it adds/modifies a configuration string <p_config> & <p_parameter>  */
/*      that belong to block <p_blockname>.                                 */
/*   2. If <p_blockname> does not exists in the current configuration file, */
/*      then it adds a new block call <p_blockname> and a configuration     */
/*      string <p_config> & <p_parameter> at the end of the file.           */
/* Arguments:                                                               */
/*	 1. p_blockname - the configuration block								*/
/*   2. p_config    - the configuration string                              */
/*   3. p_parameter - the new parameter                                     */
/* Modified:                                                                */
/*   1. p_parameter of the specified p_config                               */
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* Remark :                                                                 */
/*   1. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_PutConfigString(char *p_blockname, char *p_config,
                          char *p_parameter)
{
    char buffer[BUFSIZ+1];
    char *ptr, *bkptr;
    short status;
    short ii, ll, blkno, bIsRemark=0;
    long  config_start_posn;
    int   iNum=0;

    status = READERR;
    blkno = -1;

	/* search through the cfg_table for block identified by <p_blockname>	*/
    for (ii=0; ii<cfg_count; ii++)
    {
        // fprintf(stderr, "%d Blkname %s\n", ii, cfg_table[ii].prf_blk_name);
        if (strcmp(p_blockname, cfg_table[ii].prf_blk_name) == 0)
        {
            blkno = ii;
            ii = cfg_count + 1;
        }
    }

    if (blkno == -1)
    {
        // block does not exist, new block will be created at the end of the
        // current file pointed by fp_config.
		sprintf_s(strDebugMessage_MtnConfig, 512, "Warning: FILE: %s, LINE: %d, Block does not exist! New Block is added.\n", 
			__FILE__, __LINE__);
        if (fp_config != NULL)
            fclose(fp_config);
        fopen_s(&fp_config, cfg_file_pathname, "a");
        if (fp_config == NULL)
        {
            sprintf_s(strDebugMessage_MtnConfig, 512, "[%s %d]: Open file %s error!\n", __FILE__, __LINE__, cfg_file_pathname);
            return MTN_API_ERROR_OPEN_FILE;
        }
        fprintf(fp_config,"[%s]\n", p_blockname);
        mtn_cfg_Count_String(p_parameter, &iNum, " ,");
        if (iNum > 1)
            fprintf(fp_config,"%s = <%s>\n", p_config, p_parameter);
        else
            fprintf(fp_config,"%s = %s\n", p_config, p_parameter);

        fclose(fp_config);
        // Open the new Configuration file
        mtn_cfg_OpenConfigFile(cfg_file_pathname);
        return MTN_API_OK_ZERO;
    }
    else // block exists, check if config exist or not
    {
        status = READERR;

        /* the desired block found, now, move the file pointer to the       */
		/* starting line for that block 									*/
        fseek(fp_config, cfg_table[blkno].start_file_pos, SEEK_SET);

		/* try to locate the configuration string within the number of line */
		/* of the block, and then read it value 							*/
        for (ll=0; ll<cfg_table[blkno].num_of_lines && status == 0; ll++)
        {
            // Get the current file position
            config_start_posn = ftell(fp_config);

            // fprintf(stderr, "Start Posn %d\n", config_start_posn);

            if (fgets(buffer, BUFSIZ, fp_config) != NULL)
            {
                //  Added 30Oct2003
                ptr = &buffer[0];
                while(ptr[0] == ' ' && ptr[0] != '\0')
                    ptr++;

                if (*ptr != '#' && *ptr != ';')
                {
                    bIsRemark = 0;
                    /* this is not a remark line, locate the desired string */
					/* if string found, check for equal sign then read the	*/
					/* parameter											*/
					bkptr = ptr;
                    ptr = strstr(ptr, p_config);
                    if (ptr != NULL)
                    {
                        //  Modified 6Oct2005 to exactly match the config string
                        if(ptr != bkptr)
                        {
                            continue;
                        }

                        //  Added 5Jan2004 to exactly match the
                        // config string
                        ptr = ptr + strlen(p_config);
                        if (ptr[0] != ' ' && ptr[0] != '=')
                        {
                            continue;
                        }
                        // Config Exist within this block!
                        // Overwrite this line
                        // fprintf(stderr, "Config %s Exist\n", p_config);

                        // modify string in current file
                        if(mtn_cfg_modify_string(fp_config, config_start_posn,
                                             p_config, p_parameter) != MTN_API_OK_ZERO)
                        {
                            sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Fail to Modify String!\n",
								__FILE__, __LINE__);
                            return MTN_API_ERR_MODIFY_STRING;
                        }

                        status = READOK;
					}	/* if string found */
				}	/* if not remark line */
                else
                {
                    bIsRemark = 1; // this is remark line
                }
            }   /* if fgets ... */
        }   /* for loop */

        if (ll>=cfg_table[blkno].num_of_lines && status == 0)
        {
            // Config not found within this block!
            // need to add this config at the last line of the block

            // The new config string will inserted 1 line above
            // the remark line
            if (bIsRemark == 0)
            {
                if (!feof(fp_config))
                {
                    // Get the current file position
                    config_start_posn = ftell(fp_config);
                }
            }
            if(mtn_cfg_insert_string(fp_config, config_start_posn,
                                 p_config, p_parameter) != MTN_API_OK_ZERO)
            {
                sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Fail to Insert New String!\n", __FILE__, __LINE__);
                return MTN_API_ERR_INSERT_STRING;
            }
            status = READOK;
        }
    }
    return (status);
}

/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfginsert_text(FILE *fpSrc, long posn, char *p_config,        */
/*                           char *p_parameter)                             */
/* Purpose :                                                                */
/*   1. to insert a text string & its parameter in the current              */
/*      file.                                                               */
/* Arguments:                                                               */
/*   1. fpSrc       - Current configuration file pointer                    */
/*   2. posn        - the file position where the config string is          */
/*                    to be located                                         */
/*   3. p_config    - the new configuration string in the configuration block */
/*   4. p_parameter - the new parameter                                     */
/* Return:                                                                  */
/*   0 - not OK, 1 - OK                                                     */
/* Remark :                                                                 */
/*   1. This function is called within mtn_cfgPutConfigString().               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_insert_text(FILE *fpSrc, long posn, char *p_config, char *p_parameter)
{
    char buffer[BUFSIZ+1], filename[12];
    long curr_posn;
    FILE *fpDest;

    // open a temporary file
    fpDest = NULL;
    tmpnam(filename);
    fopen_s(&fpDest, filename, "w");
    if(fpSrc==NULL || fpDest == NULL)
    {
        sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d :Invalid file pointer!\n", __FILE__, __LINE__);
        return MTN_API_ERR_FILE_PTR;
    }

    // Point to the start position of the file
    fseek(fpSrc, 0, SEEK_SET);
    while (!feof(fpSrc))
    {
        curr_posn = ftell(fpSrc);
        if(fgets(buffer, BUFSIZ, fpSrc) != NULL)
        {
            if (curr_posn!=posn)
            {
                fprintf(fpDest, "%s", buffer);
            }
            else
            {
                fprintf(fpDest, "%s", buffer);
                fprintf(fpDest, "# %s <%s>\n", p_config, p_parameter);
            }
        }
        // Handling for Block located at the last line
        if (feof(fpSrc))
        {
            curr_posn = ftell(fpSrc);
            if (curr_posn==posn)
            {
                fprintf(fpDest, "# %s <%s>\n", p_config, p_parameter);
            }
        }
    }
    fclose(fpSrc);
    fclose(fpDest);
    remove(cfg_file_pathname);
    if (rename(filename, cfg_file_pathname) !=0)
    {
        sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Rename %s to be %s failed!\n", 
			__FILE__, __LINE__, filename, cfg_file_pathname);
        return MTN_API_ERR_RENAME_FILE;
    }

    // Open the New Configuration file
    mtn_cfg_OpenConfigFile(cfg_file_pathname);
    return MTN_API_OK_ZERO;
}


/* ------------------------------------------------------------------------ */
/* Function :                                                               */
/*   short mtn_cfgPutText(char *p_blockname, char *p_config,                */
/*		 char *p_parameter) 												*/
/* Purpose :                                                                */
/*   1. If <p_blockname> exists in the current configuration file, then     */
/*      it adds a text string <p_config> & <p_parameter>                    */
/*      that belong to block <p_blockname>.                                 */
/*   2. If <p_blockname> does not exists in the current configuration file, */
/*      then it adds a new block call <p_blockname> and a text              */
/*      string <p_config> & <p_parameter> at the end of the file.           */
/* Arguments:                                                               */
/*   1. p_blockname - the text block                                        */
/*   2. p_config    - the text string                                       */
/*   3. p_parameter - the new parameter                                     */
/* Modified:                                                                */
/*   1. p_parameter of the specified p_config                               */
/* Return:                                                                  */
/*	 0 - not OK, 1 - OK 													*/
/* Remark :                                                                 */
/*   1. mtn_cfg_OpenConfigFile should be called sucessfully first               */
/* ------------------------------------------------------------------------ */
short mtn_cfg_PutText(char *p_blockname, char *p_config,
                      char *p_parameter)
{
    short status;
    short ii, blkno;
    long  config_start_posn;

    status = READERR;
    blkno = -1;

	/* search through the cfg_table for block identified by <p_blockname>	*/
    for (ii=0; ii<cfg_count; ii++)
    {
        // fprintf(stderr, "%d Blkname %s\n", ii, cfg_table[ii].prf_blk_name);
        if (strcmp(p_blockname, cfg_table[ii].prf_blk_name) == 0)
        {
            blkno = ii;
            ii = cfg_count + 1;
        }
    }

    if (blkno == -1)
    {
        // block does not exist, new block will be created at the end of the
        // current file pointed by fp_config.
        // fprintf(stderr, "Block does not exist! New Block is added.\n");
        if (fp_config != NULL)
            fclose(fp_config);
        fopen_s(&fp_config, cfg_file_pathname, "a");
        if (fp_config == NULL)
        {
            sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Open file %s error!\n", __FILE__, __LINE__, cfg_file_pathname);
            return MTN_API_ERROR_OPEN_FILE;
        }
        fprintf(fp_config,"# [%s]\n", p_blockname);
        fprintf(fp_config,"# %s <%s>\n", p_config, p_parameter);

        fclose(fp_config);
        // Open the new Configuration file
        mtn_cfg_OpenConfigFile(cfg_file_pathname);
        return MTN_API_OK_ZERO;
    }
    else // block exists
    {
        /* the desired block found, now, move the file pointer to the       */
		/* starting line for that block 									*/
        fseek(fp_config, cfg_table[blkno].start_file_pos, SEEK_SET);

        // Get the current file position
        config_start_posn = ftell(fp_config);

        // The new text will inserted 1 line after the Block
        if(mtn_cfg_insert_text(fp_config, config_start_posn,
                               p_config, p_parameter) != MTN_API_OK_ZERO)
        {
            sprintf_s(strDebugMessage_MtnConfig, 512, "FILE: %s, LINE: %d, Fail to Insert New String!\n",
				__FILE__, __LINE__);
            return MTN_API_ERR_INSERT_STRING;
        }
        status = READOK;
    }
    return (status);
}

// ****************************************************************
// Stub program to test the functionality.
// compilation: wcl386 /w5 /k100k config.c
#ifdef __STUB__
void main(void)
{
    char    file1[12] = "test.ini";
    char    param[128];
    char    **buffer;
    int     ii=0, cycle=0;

#ifdef LOOP_TEST
while(!kbhit())
{
    if (ii>=10000)
    {
        ii=0;
        cycle++;
    }
    printf("Cycle %d: ", cycle);
    if (mtn_cfg_OpenConfigFile(file1)!=1)
    {
        printf("Open config file %s error!\n", file1);
        return;
    }

    // *******************************************
    // Modify current String
    if (mtn_cfgReadConfigString("mno", "prof1", param) != MTN_API_OK_ZERO)
    {
        printf("Unable to get string\n");
    }
    printf("Old Value = %s;",param);

    sprintf(param, "%d ;remark1", ii++);
    if (mtn_cfgPutConfigString("mno", "prof1", param) != MTN_API_OK_ZERO)
    {
        printf("Fail in Putting string\n");
        // getch();
    }

    sprintf(param, "%d", 0);
    if (mtn_cfgReadConfigString("mno", "prof1", param)!= MTN_API_OK_ZERO)
    {
        printf("Unable to get string\n");
    }
    printf(" New Value = %s; ",param);

    // **************************************************
    // Insert String
    sprintf(param, "%d", 0);
    if (mtn_cfgReadConfigString("mno", "prof4", param)!= MTN_API_OK_ZERO)
    {
        printf("Unable to get string\n");
    }
    printf("Old Value = %s; ",param);

    sprintf(param, "%d ;remark2", ii*2);
    if (mtn_cfgPutConfigString("mno", "prof4", param) != MTN_API_OK_ZERO)
    {
        printf("Fail in Putting string\n");
    }

    sprintf(param, "%d", 0);
    if (mtn_cfgReadConfigString("mno", "prof4", param)!= MTN_API_OK_ZERO)
    {
        printf("Unable to get string\n");
    }
    printf("New Value = %s\r",param);

    mtn_cfgCloseConfigFile();
}
#else
    if (mtn_cfg_OpenConfigFile(file1)!=1)
    {
        printf("Open config file %s error!\n", file1);
        return;
    }

    if (mtn_cfgReadConfigString("mno", "prof4", param) != MTN_API_OK_ZERO)
    {
        printf("Unable to get string\n");
    }
    printf("Old Value = %s\n",param);

    sprintf(param, "%s", "9988, 2.0, $33 , 1.");
    if (mtn_cfgPutConfigString("mno", "prof4", param) != MTN_API_OK_ZERO)
    {
        printf("Fail in Putting string\n");
        // getch();
    }

    sprintf(param, "%d", 0);
    if (mtn_cfgReadConfigString("mno", "prof4", param)!= MTN_API_OK_ZERO)
    {
        printf("Unable to get string\n");
    }
    printf("New Value = %s\n",param);

    mtn_cfg_Count_String(param, &ii, " ,");
    printf("Token Count = %d\n", ii);

#endif // LOOP_TEST

}
#endif // __STUB__

