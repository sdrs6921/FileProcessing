#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

BlockMapTbl blkmaptbl;

extern devicefp;
/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void initialize_flash_memory();
void print_block(int pbn);
void print_blkmaptbl();

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		blkmaptbl.entry[i].pbn = -1;
		blkmaptbl.entry[i].first_free_page_offset = 0;
	}

	//
	// 추가적으로 필요한 작업이 있으면 수행할 것
	//

	return;
}

void ftl_write(int lsn, char *sectorbuf)
{
	SpareData sprdat = { lsn, 0, "" };
	char pagebuf[PAGE_SIZE] = "";
	char lsnStr[4] = "";
	char invalidStr[4] = "";
	static int excpet_block_index = BLOCKS_PER_DEVICE - 1;
	int i, j, ppn, check, tmp;

	if(blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset < PAGES_PER_BLOCK) {
		if (blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn == -1)//pbn이 존재하지 않는 경우에만
			blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn = (int)(lsn / PAGES_PER_BLOCK);
		ppn = (blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn * PAGES_PER_BLOCK) +
			blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset;
		
		
		strcpy(lsnStr, &(sprdat.lsn));
		strcpy(invalidStr, &(sprdat.invalid));
			
		strcpy(pagebuf , sectorbuf);
		strcpy(pagebuf + SECTOR_SIZE, lsnStr);
		strcpy(pagebuf + SECTOR_SIZE + 4, invalidStr);
		strcpy(pagebuf + SECTOR_SIZE + 8, sprdat.dummy);
		
		
		dd_write(ppn , pagebuf);

		blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset++;

		/*change same_lsn spare data*/
		for (i = 0; i < blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset - 1; i++)
		{
			dd_read((blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn*BLOCK_SIZE) + i,pagebuf);
			strcpy(lsnStr, pagebuf + SECTOR_SIZE);

			check = (*lsnStr);
			
			if (check == lsn) {
				sprdat.invalid = TRUE;
				strcpy(invalidStr, &(sprdat.invalid));
				strcpy(pagebuf + SECTOR_SIZE + 4,invalidStr);
				dd_write((blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn*BLOCK_SIZE) + i ,pagebuf);
			}
			
			
		}
	}
	else {
		//갓 들어온 정보 먼저 할당
		strcpy(lsnStr, &(sprdat.lsn));
		strcpy(invalidStr, &(sprdat.invalid));

		ppn = (excpet_block_index) * PAGES_PER_BLOCK;
		
		strcpy(pagebuf, sectorbuf);
		strcpy(pagebuf + SECTOR_SIZE, lsnStr);
		strcpy(pagebuf + SECTOR_SIZE + 4, invalidStr);
		strcpy(pagebuf + SECTOR_SIZE + 8, sprdat.dummy);

		dd_write(ppn, pagebuf);


		for (i = 0; i < blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset; i++)
		{
			dd_read((blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn*BLOCK_SIZE) + i, pagebuf);
			for (j = 0; j < 4; j++) {
				lsnStr[j] = pagebuf[j + SECTOR_SIZE];
			}

			check = (*lsnStr);

			if (check == lsn) {
				/*check lsn*/
				sprdat.invalid = TRUE;
				strcpy(invalidStr, &(sprdat.invalid));
				strcpy(pagebuf + SECTOR_SIZE + 4, invalidStr);
				dd_write((blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn*BLOCK_SIZE) + i, pagebuf);
			}

					
		}

		blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset = 1;

		
		for (i = 0; i < PAGES_PER_BLOCK; i++) {
			
			dd_read((blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn*PAGES_PER_BLOCK) + i, pagebuf);

			for (j = 0; j < 4; j++)
			{
				invalidStr[j] = pagebuf[j + 4 + SECTOR_SIZE];
			}

			check = (*invalidStr);

			if (check == FALSE){
				dd_write((excpet_block_index)*(PAGES_PER_BLOCK) + blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset-1, pagebuf);
				blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset++;
			}
		}

		dd_erase(blkmaptbl.entry[lsn/PAGES_PER_BLOCK].pbn);
		//change pbn<->blank block
		tmp = (excpet_block_index);
		(excpet_block_index) = blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn;
		blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn = tmp;

	}
	
#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl();
	print_block(31);
#endif

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	SpareData sprdat = { lsn, 0, "" };
	char pagebuf[PAGE_SIZE] ="";
	char lsnStr[4] ="";
	char invalidStr[4] = "";

	int i , j ,check;

	for (i = 0; i < blkmaptbl.entry[lsn / PAGES_PER_BLOCK].first_free_page_offset; i++)
	{
		dd_read((blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn*PAGES_PER_BLOCK) + i, pagebuf);

		strcpy(lsnStr, &(sprdat.lsn));
		strcpy(invalidStr, &(sprdat.invalid));

		if (strncmp(pagebuf + SECTOR_SIZE, lsnStr, sizeof(int)) == 0) {
			check = (*invalidStr);
			if (strncmp(pagebuf + SECTOR_SIZE + 4, lsnStr, sizeof(int)) == 0) {
				strcpy(sectorbuf, pagebuf);
				break;
			}
		}
	}
	
#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl();
	print_block(blkmaptbl.entry[lsn / PAGES_PER_BLOCK].pbn);
#endif

	return;
}

//
// initialize flash memory where each byte are set to 'OxFF'
// 
void initialize_flash_memory()
{
	char *blockbuf;
	int i;

	blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	free(blockbuf);

	return;
}

void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%5d][%5d]\n", i, sdata->lsn, sdata->invalid);
	}

	free(pagebuf);
	free(sdata);

	return;
}

void print_blkmaptbl()
{
	int i;

	printf("Block Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(blkmaptbl.entry[i].pbn >= 0)
		{
			printf("[%d %d %d]\n", i, blkmaptbl.entry[i].pbn, blkmaptbl.entry[i].first_free_page_offset);
		}
	}
}
