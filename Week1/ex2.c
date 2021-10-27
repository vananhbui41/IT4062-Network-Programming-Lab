#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 50

struct subjectList{
    char id[MAX];
    char name[MAX];
    int midFactor;
    char semester[MAX];
    int studentCount;
};

struct studenList
{
    char id[MAX];
    char firstName[MAX];
    char lastName[MAX];
    float mid;
    float last;
    float final;
    char rank;
    struct studenList *next;
};

typedef struct subjectList Subject;
typedef struct studentList Student;

void getString(char *str){
    fgets(str,100,stdin);
    str[strlen(str)-1] = '\0';
}

Subject *createSubject(char id[], char name[],int midFactor,char semester[],int studentCount){
    Subject*new = (Subject*) malloc(sizeof(Subject));
    strcpy(new->id,id);
    strcpy(new->name,name);
    new->midFactor = midFactor;
    strcpy(new->semester,semester);
    new->studentCount = studentCount;
    return new;
}

void writeToFileSubject(Subject*subject){
    char file_name[MAX];
    FILE*fptr;
    if(subject != NULL){
        strcpy(file_name,subject->id);
        strcat(file_name,"_");
        strcat(file_name,subject->semester);
        strcat(file_name,".txt");
        if((fptr = fopen(file_name,"w+")) == NULL){
            printf("Can not open file!\n");
            return;
        }
        fprintf(fptr,"SubjectID|%s\n",subject->id );
        fprintf(fptr,"Subject|%s\n",subject->name);
        fprintf(fptr,"F|%d|%d\n",subject->midFactor,100-subject->midFactor);
        fprintf(fptr,"Semester|%s\n",subject->semester);
        fprintf(fptr,"Student Count|%d",subject->studentCount);
        fclose(fptr);
    }
}

void addNewScoreBoard(Subject**list){
    char id[MAX], name[MAX],semester[MAX];
    int midFactor, studentCount;
    printf("Enter subject information:\n");
    while (getchar() != '\n');
    printf("Subject ID: \n");
    getString(id);
    printf("Subject Name: \n");
    getString(name);
    do {
        printf("Enter mid-term factor (0 =<x =<100) :\n");
		scanf("%d",&midFactor);
    }while (midFactor<=0 || midFactor >=100);
    while (getchar() != '\n');
    printf("Semester: \n");
    getString(semester);
    printf("Student count:\n");
	scanf("%d",&studentCount);
    Subject* Subject = createSubject(id,name,midFactor,semester,studentCount);
    writeToFileSubject(Subject);   
}


void addScore(){

}

int main(int argc, char const *argv[]){
    Subject* list = NULL;
    int x;
    char c;
    do{
        printf("Learning Management System\n-------------------------------------");
        printf("1.\tAdd a new score board\n");
        printf("2.\tAdd score\n");
        printf("3.\tRemove core\n");
        printf("4.\tSearch score\n");
        printf("5.\tDisplay score board and score report\n");
        printf("Other key: Quit\n");
        scanf("%d",&x);
        switch (x)
        {
        case 1:
            addNewScoreBoard(&list);
            break;
        
        default:
            break;
        }
    } while (x>0 && x<6);
    return 0;
}
