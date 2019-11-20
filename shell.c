/* Ian Calhoun
   Sean Butrica
   Comp 
   11/14/2019 */

#include <stdio.h>

int main()
{
  char line[83], filename[10], temp[2];
  int sectorsRead = 0, i, j;
  char buffer[13312];
  
  syscall(0,"SHELL>: ");
  syscall(1, line);

  if((line[0] == 't')&&(line[1] == 'y')&&(line[2] == 'p')&&(line[3] == 'e'))
    {

      syscall(3, line+5, buffer, &sectorsRead);
      
      if(sectorsRead != 0)
	{
	  syscall(0, buffer);
	}
      else
	{
	  syscall(0, "invalid file");
	}

    }
  else if((line[0] == 'e')&&(line[1] == 'x')&&(line[2] == 'e')&&(line[3] == 'c'))
    {

      syscall(4, line+5);

    }
  else if((line[0] == 'd')&&(line[1] == 'i')&&(line[2] =='r'))
    {
      syscall(2, buffer, 2);

      for(i = 0; i <= 512; i = i + 32)
	{
	  if(buffer[i] != "\0")
	    {
	      for(j = 0; j< 6;j++)
		{
		      filename[j] = buffer[i+j];
		}
	      filename[6] = 0xa;
	      filename[7] = 0xd;
	      filename[8] = 0x0;
	    }
	  syscall(0, filename);
	}
	  temp[0] = 0xa;
	  temp[1] = 0xd;
	  temp[2] = 0x0;
	  syscall(0, temp);
	
    }
}

