#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define MAX 50

//Dạng hóa chât //kí tự 1
struct 
{
    int number;
    char material[MAX];
} form[4] = {
    {1, "Jets"},
    {2, "Fog"},
    {3, "Foam"},
    {4, "Dry agent"}
};

//Kí tự 2
struct char2 
{
    char c;
    char color;
    char Reactivity;
    char Protection[MAX];
    char Containment[MAX];
} infor[12] = {
    {'P','n','V',"Full","Dilute"},
    {'R','n','\0',"Full","Dilute"},
    {'S','n','V',"BA","Dilute"},
    {'S','y','V',"BA for fire only","Dilute"},
    {'T','n','\0',"BA","Dilute"},
    {'T','y','\0',"BA for fire only","Dilute"},
    {'W','n','V',"Full","Contain"},
    {'X','n','\0',"Full","Contain"},
    {'Y','n','V',"BA","Contain"},
    {'Y','y','V',"BA for fire only","Contain"},
    {'Z','n','V',"BA","Contain"},
    {'Z','y','V',"BA for fire only","Contain"}
};

//in ra cach xu ly
void print_result(struct char2 str){
  if (str.Reactivity == 'V')
    printf("Reactivity:\tcan be violently reactive\n");
  else printf("Reactivity: \n");
  printf("Protection:  ");
  if (strcmp(str.Protection, "Full") == 0)
    printf("full protective clothing must be worn\n");
  if (strcmp(str.Protection, "BA") == 0)
    printf("breathing apparatus, protective gloves for fire only\n");
  if (strcmp(str.Protection, "BA for fire only") == 0)
    printf("breathing apparatus, protective gloves for fire only\n");  
  printf("Containment: ");
  if (strcmp(str.Containment, "Dilute") == 0)
    printf("the dangerous goods may be washed down to a drain with a large quantity of water\n");
  if (strcmp(str.Containment, "Contain") == 0)
    printf("a need to avoid spillages from entering drains or water courses\n");
  
}

int main(){
  //Kiem tra ma HAZCHEM
  //print_result(infor[3]);
  int check = 0;
  char code[3];
  do {
      printf("Enter HAZCHEM code: ");      
      scanf("%s",code);
      if (strlen(code) == 2 || strlen(code) == 3){
          for (int i= 0 ; i< 4;i++){
            if (form[i].number == code[0] - '0'){
              for (int j=0 ; j<12 ; j++) {
                if (infor[j].c == code[1]) {
                  if (code[2] == 'E') {
                    check = 1; 
                    break;
                  } 
                  if (code[2] == '\0'){
                    check = 1;
                    break;
                  }
                  else break;
                }
              }
            }
          }
      }
    if (check) break;
    else printf("Input must be HAZCHEM code!\n");
    } while (1);
  
  //kiem tra mau
  char coloryn[3];
  do{
    printf("Is the S reverse coloured? ");
    //while (getchar() != '\n');
    scanf("%s",coloryn);
    if ( strcmp(coloryn,"no") == 0 || strcmp(coloryn,"yes") == 0) break;
    else printf("yes or no!!!\n");
  } while (1);

  char color;
  if (strcmp(coloryn,"no") == 0) color = 'n';
  else color = 'y';
  printf("***Emergency action advice***\n");
  for (int i = 0; i<4 ; i++){
    if (form[i].number == code[0] - '0'){
      printf("Material: %s\n", form[i].material);
    }
  }

  for (int i = 0; i < 12; i++){
    if ((infor[i].c == code[1]) && (infor[i].color == color)){    
       print_result(infor[i]); 
    }
  }
  if ( code[2]=='E')
    printf("Evacuation:  consider evacuation\n");
  return 0;
}
