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
