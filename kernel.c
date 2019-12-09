/* Ian Calhoun
   Sean Butrica
   COMP 350-003
   10/24/2019 */
 

void printString(char*);
void printChar(char);
void readString(char*);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);
void readFile(char*,char*,int*);
int stringCompare(char*, char*);
void executeProgram(char*);
void terminate();
void writeSector(char*, int);
void deleteFile(char*);
void writeFile(char*, char*,int);



int main()
{
  makeInterrupt21();
  interrupt(0x21, 4, "shell", 0, 0);
  while(1);
}


void printString(char* chars)
{
  while(*chars != 0x0)
    {
      interrupt(0x10, 0xe*256+*chars, 0, 0, 0);
      chars++;
    }
}


void printChar(char c)
{
  interrupt(0x10, 0xe*256+c, 0, 0, 0);
}


void readString(char* line)
{
  int count = 0;
  char temp;

  while(1)
    {
      temp = interrupt(0x16, 0, 0, 0, 0);
      if((temp != 0xd)&&(temp != 0x8))
	{
	  interrupt(0x10, 0xe*256+temp, 0, 0, 0);
	  line[count] = temp;
	  count++;
	}
      else if(temp == 0x8)
	{
	  if(count > 0)
	    {
	      interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
	      count--;
	      interrupt(0x10, 0xe*256+' ', 0, 0, 0);
	      interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
	    }
	}
      else
	{
	  line[count+1] = 0xa;
	  interrupt(0x10, 0xe*256+line[count+1], 0, 0 ,0);
	  line[count+2] = 0x0;
	  interrupt(0x10, 0xe*256+line[count+2], 0, 0, 0);
	  interrupt(0x10, 0xe*256+temp, 0, 0, 0);
	  return;
	}
    }
}


void readSector(char* buffer, int sector)
{
  int relative_sec = sector + 1, track= 0, head=0;
   
  interrupt(0x13, 2*256+1, buffer, track*256+relative_sec, head*256+0x80);
}


void handleInterrupt21(int ax, int bx, int cx, int dx)
{
  if(ax == 0)
    {
      printString(bx);
    }
  else if(ax == 1)
    {
      readString(bx);
    }
  else if(ax == 2)
    {
      readSector(bx,cx);
    }
  else if(ax == 3)
    {
      readFile(bx,cx,dx);
    }
  else if(ax == 4)
    {
      executeProgram(bx);
    }
  else if(ax == 5)
    {
      terminate();
    }
  else if(ax == 6)
    {
      writeSector(bx, cx);
    }
  else if(ax == 7)
    {
      deleteFile(bx);
    }
  else if(ax == 8)
    {
      writeFile(bx,cx,dx);
    }
  else
    {
      printString("that is an incorrect call");
    }
}


void readFile(char* filename, char* buffer,int* sectorsRead)
{
  int fileentry, i, c, check = 0;
  char temp[6];
  char dir[512];

  readSector(dir,2);
  
  for(fileentry = 0; fileentry < 512; fileentry = fileentry+32)
    {
        for(i = 0; i < 6; i++)
	{
	  temp[i] = dir[fileentry+i]; 
	}
	check = stringCompare(temp, filename);
	if(check != 0)
	  break;
    }

  if(check == 0)
    {
      *sectorsRead = 0;
      return;
    }
  else
    {
      for(c = fileentry+6; c < fileentry+32 ; c ++)
	{
	  readSector(buffer,dir[c]);
	  *sectorsRead = *sectorsRead + 1;
	  buffer = buffer + 512;
	}
    }
}


int stringCompare(char* message1, char* message2)
{
  int count;
  for(count = 0; count <= 5;count++)
    {
      if(message1[count] != message2[count])
	{
	  return 0;
	}
      else if(message1[count] == '\0')
	{
	  break;
	}
      else if(message1[count] == '\r')
	{
	  break;
	}
    }
  return 1;
}


void executeProgram(char* name)
{
  char buffer[13312];
  int* sectors;
  int count = 0, memStart = 0x2000;
  int offset = 0x0;
  readFile(name, buffer, &sectors);
 
  if(sectors = 0)
    {
      printString("No such program.");
    }
  else
    {
      for(count; count < 13312;count++)
	{
	  putInMemory(memStart, offset, buffer[count]);
	  offset = offset + 1;
	}
    }
  launchProgram(memStart);
}


void terminate()
{
  char shellname[6];

  shellname[0] = 's';
  shellname[1] = 'h';
  shellname[2] = 'e';
  shellname[3] = 'l';
  shellname[4] = 'l';
  shellname[5] = '\0';

  executeProgram(shellname);
  
  while(1);
}

void writeSector(char* buffer, int sector)
{
  int relative_sec = sector + 1, track= 0, head=0;

  interrupt(0x13, 3*256+1, buffer, track*256+relative_sec, head*256+0x80);
}



void deleteFile(char* filename)
{
  char dir[512], map[512], temp[6];
  int i, j, match = 0, count = 6;
  
  readSector(dir, 2);
  readSector(map, 1);
  
  for(i = 0; i <=512; i =  i + 32)
    {
      if(dir[i] != 0)
	{
	  for(j = 0; j < 6; j++)
	    {
	      temp[j] = dir[i+j];
	    }
	}
      match = stringCompare(temp, filename);
      if(match != 0)
	{
	  dir[i] = 0x0;
	  while(dir[i+count] != 0x0)
	    {
	      map[dir[i+count]] = 0x0;
	      count = count + 1;
	    }
	}
    }
  writeSector(dir,2);
  writeSector(map,1);
}

void writeFile(char* buffer, char* filename, int numberOfSectors)
{
  char map[512], dir[512], tempName[10];
  int i, j, k, l, m, count = 6;

  readSector(map, 1);
  readSector(dir, 2);

  for(i = 0; i < 512; i = i + 32)
    {
      if(dir[i] == 0x0)
	{
	  for(j = 0; j < 6; j++)
	    {
	      if(filename[j] != 0x0)
		{
		  dir[i+j] = filename[j];
		}
	      else
		{
		  dir[i+j] = 0x0;
		}
	    }
	      break;
	    }
    }
  for(l = 0; l < numberOfSectors; l++)
    {
      for(k = 3; k <= 512; k++)
	{
	  if(map[k] == 0)
	    {
	      map[k] = 0xFF;
	      dir[i+count] = k;
	      writeSector(buffer,k);
	      buffer = buffer + 512;
	      count = count + 1;
	      break;
	    }
	}
    }
  writeSector(map, 1);
  writeSector(dir, 2);
}
