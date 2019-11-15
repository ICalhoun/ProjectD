/* Ian Calhoun
   Sean Butrica
   Comp 
   11/14/2019 */

#include <stdio.h>

int main()
{
  char line[83];
  int sectorsRead = 0;
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
}

