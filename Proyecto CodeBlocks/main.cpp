#include <stdio.h>
//#include <stdlib.h>
#include <graphics.h>
//#include <windows.h>
#include <time.h>
#include <string.h>

#define grilla 35

typedef struct
{
    int x;
    int y;
} Cordenadas;

typedef struct
{
    int x1;
    int x2;
    int y1;
    int y2;
} pixelesGrilla;

void musicaDJ(void);
void crearGrillaMatriz(int ***M,pixelesGrilla dim);
int cargarGrillas(int **Minas,int **Mostrar,pixelesGrilla dim);
void mostrarGrillaCargada(int **Minas,pixelesGrilla dim);
void completarMatrizMinas(int **Minas,pixelesGrilla dim);
void dibujarParteInferior(pixelesGrilla inferior);
void agregarParteInferiorMatrices(int ***Minas,int ***Mostrar);
void efectoFin(pixelesGrilla Dim,int jugar);
void dibujarGrilla(int cantidadC, int cantidadF);
void grillasBMP(Cordenadas mouse,int aux);
void contadorMinas(pixelesGrilla inferior,int contador);
int contarPasados(int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim);
void iniciarPrimerIntento(int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim);
void guardarScore(int puntaje,pixelesGrilla modo);
void mostrarScore(void);
int iniciarJuego(int **grillaMinas,int **grillaMostrar,int cantidadF,int ccantidadC);
void Presentacion(void);
int contarArchi(void);
int menu(pixelesGrilla *grillaDim);
void posicionMouse(Cordenadas *mouse);
void grillaAflagMejorado(Cordenadas mouse,int **grillaMostrar,int *);
int botonIzquierdoGrilla(Cordenadas mouse,int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim);
void perdiste(int **grillaMinas,int ** grillaMostrar,pixelesGrilla Dim);


int main()
{
    //ShowWindow(GetForegroundWindow(),SW_HIDE);
    /// PRESENTACION
    Presentacion();

    /// CARGA MUSICA EN LOOP
    musicaDJ();

    /// SECTOR VARIABLES
    int ** grillaMinas=NULL;
    int ** grillaMostrar=NULL;
    Cordenadas mouse;
    pixelesGrilla grillaDim;
    int menuContinuar=1;
    int jugar,minasCargadas,puntos,puntaje;
    /// LOOP DE JUEGO DESDE EL MENU
    while (menuContinuar==1)
    {

        /// MENU
        minasCargadas=puntos=puntaje=0;
        jugar=1;
        puntos = menu(&grillaDim);

        /// GENERACION DE MATRICES DEPENDIENDO LA DIFICULTAD
        crearGrillaMatriz(&grillaMinas,grillaDim);
        crearGrillaMatriz(&grillaMostrar,grillaDim);
        minasCargadas = cargarGrillas(grillaMinas,grillaMostrar,grillaDim);
        completarMatrizMinas(grillaMinas,grillaDim);

        /// CORDENADAS PARA CENTRAR LA PANTALLA  (DEL JUEGO)
        DWORD screenX = ((GetSystemMetrics(SM_CXSCREEN)/2) - (grillaDim.x2 / 2));
        DWORD screenY = ((GetSystemMetrics(SM_CYSCREEN)/2) - (grillaDim.y2 / 2));

        /// INICIAR LA PANTALLA CON EL TAMAÑO ELEGIDO
        initwindow(grillaDim.x2,grillaDim.y2+(grilla*2),"UT MINAS",screenX,screenY);

        /// DIBUJAR GRILLA
        dibujarGrilla(grillaDim.x1,grillaDim.y1);

        /// PARTE INFERIOR
        dibujarParteInferior(grillaDim);

        /// LOOP DEL JUEGO
        iniciarPrimerIntento(grillaMostrar,grillaMinas,grillaDim);
        while(jugar!=0 && puntaje!=puntos)
        {
            puntaje = contarPasados(grillaMostrar,grillaMinas,grillaDim);
            posicionMouse(&mouse);
            if (mouse.x<grillaDim.x1)
            {
                if (GetAsyncKeyState(VK_LBUTTON) && ((grillaMostrar[mouse.x][mouse.y]!=11) && (grillaMostrar[mouse.x][mouse.y]!=20)))
                {
                    jugar = botonIzquierdoGrilla(mouse,grillaMostrar,grillaMinas,grillaDim);
                }
                if (GetAsyncKeyState(VK_RBUTTON))
                {
                    grillaAflagMejorado(mouse,grillaMostrar,&minasCargadas);
                }
                contadorMinas(grillaDim,minasCargadas);
                delay(120);
            }
        }
        guardarScore(puntaje,grillaDim);
        perdiste(grillaMinas,grillaMostrar,grillaDim);
        delay(500);
        efectoFin(grillaDim,jugar);
        closegraph();
        free(grillaMinas);
        free(grillaMostrar);
        free(*grillaMinas);
        free(*grillaMostrar);
    }


}
///FIN MAIN

void cargaScore(){
    int jugadas = contarArchi();
    char modo1[]="FACIL   ";
    char modo2[]="MEDIO  ";
    char modo3[]="DIFICIL ";
    char tabla[20];
    Cordenadas score[10];
    FILE * aux = fopen("score.dat","rb");
    if (jugadas>10){
    fseek(aux,(jugadas-10)*sizeof(Cordenadas),SEEK_CUR);
    jugadas=10;
    }
    fread(&score,sizeof(Cordenadas),jugadas,aux);
    settextstyle(BOLD_FONT,HORIZ_DIR,2);
    outtextxy(90,10,"T O P 10");
    for (int i=0;i<jugadas;i++){
        if (score[i].x<10){
                sprintf(tabla,"  00%d    ",score[i].x);
        }
        else if (score[i].x<100)
        {
                sprintf(tabla,"  0%d    ",score[i].x);
        }
        else
        sprintf(tabla,"  %d    ",score[i].x);

        switch(score[i].y)
        {
        case 8:
                strcat(tabla,modo1);
                break;
        case 12:
                strcat(tabla,modo2);
                break;
        case 16:
                strcat(tabla,modo3);
                break;
        }
        outtextxy(50,320-(30*i),tabla);
    }
}




/// GUARDA EN UN ARCHIVO LA ULTIMA JUGADA " EL PUNTAJE ES LA CANTIDAD DE GRILLAS QUE SE VAN DESBLOQUEANDO
/// Y EL MODO ES LA CANTIDAD DE GRILLAS DEPENDIENDO ELNIVEL, SE GUARDA COMO ENTERO Y DESPUES SE EVALUA
void guardarScore(int puntaje,pixelesGrilla modo)
{
    FILE * aux;
    aux = fopen("score.dat","ab");
    fwrite(&puntaje,sizeof(Cordenadas),1,aux);
    fclose(aux);
}

/// DEVUELVE LA CANTIDAD DE JUGADAS GUARDADAS EN EL ARCHIVO
int contarArchi(void){
    int largo;
    FILE * aux = fopen("score.dat","rb");
    fseek(aux,0,SEEK_END);
    largo = ftell(aux);
    largo/=sizeof(Cordenadas);
    fclose(aux);
    return largo;
}

/// MUESTRA LOS ULTIMOS 10 ( SUPER BETA
void mostrarScore(void)
{
    FILE * aux;
    Cordenadas aLeer;
    int cont=0;
    aux = fopen("score.dat","rb");
    while (fread(&aLeer,sizeof(Cordenadas),1,aux)>0 && cont<10)
    {
        printf("\npuntaje: %d\n",aLeer.x);
        printf("modo: %d\n",aLeer.y);
        printf("\n");
        cont++;
    }
    fclose(aux);
}


///EFECTO QUE LIMPIA LA PANTALLA APARECE UN CARTEL SI GANASDTE O PERDISTE
/// TAMBIEN VA A DAR LA OPCION DE CONTINUAR O SALIR
void efectoFin(pixelesGrilla Dim,int jugar)
{
    srand(time(NULL));
    int aux = Dim.x1 * (Dim.y1+2);
    int posiciones=0;
    Cordenadas efecto;
    pixelesGrilla auxGrilla;
    int Matrix[Dim.x1][Dim.y1+2]= {0};
    while (posiciones!= aux)
    {
        ///SOLUCION A BUG,EL RANDOM NO TIRA UNA POSICION DE LA GRILLA Y QUEDA UN LOOP INFINITO
        if (posiciones==aux-1)
            posiciones++;
        ///
        efecto.x = rand()%Dim.x1;
        efecto.y = rand()%(Dim.y1+2);
        if  (Matrix[efecto.x][efecto.y] != 1)
        {
            auxGrilla.x1 = efecto.x * grilla;
            auxGrilla.y1 = efecto.y * grilla;
            auxGrilla.x2 = auxGrilla.x1 + grilla;
            auxGrilla.y2 = auxGrilla.y1 + grilla;
            readimagefile("Imagenes\\grillaFlag.bmp",auxGrilla.x1,auxGrilla.y1,auxGrilla.x2,auxGrilla.y2);
            Matrix[efecto.x][efecto.y] = 1;
            posiciones++;
        }
    }
    int auxCartelLetra;
    switch(Dim.x1)
    {
    case 8:
        auxCartelLetra=4;
        break;
    case 12:
        auxCartelLetra=6;
        break;
    case 16:
        auxCartelLetra=7;
        break;
    }
    if (jugar==0)
    {
        settextstyle(BOLD_FONT,HORIZ_DIR,auxCartelLetra);
        outtextxy(5,Dim.y2/2,"P E R D I S T E");
    }
    else
    {
        settextstyle(BOLD_FONT,HORIZ_DIR,auxCartelLetra);
        outtextxy(10,Dim.y2/2,"G A N A S T E");

    }
    delay(500);
}



///MENU ASIGNA VALORES A LA DIMENSION DE LA MATRIZ DEPENDIENDO EL NIVEL DE DIFICULTAD
int menu(pixelesGrilla *grillaDim)
{
    int puntaje=0;
    Cordenadas aux;
    Cordenadas mouse;
    mouse.x=-1;
    mouse.y=-1;
    int opcion=1;
    aux.x = 8 * grilla;
    aux.y = 10 * grilla;
    /// CENTRAR MENU
    DWORD screenX = ((GetSystemMetrics(SM_CXSCREEN)/2) - (aux.x / 2));
    DWORD screenY = ((GetSystemMetrics(SM_CYSCREEN)/2) - (aux.y / 2));
    /// INICIAR MENU
    initwindow(aux.x,aux.y,"MENU",screenX,screenY);
    dibujarGrilla(8,10);
    cargaScore();

    while (mouse.x==-1 && mouse.y==-1)
    {
        if (GetAsyncKeyState(VK_LBUTTON))
            getmouseclick(WM_LBUTTONUP,mouse.x,mouse.y);
    }
    delay(100);
    dibujarGrilla(8,10);
    readimagefile("Imagenes\\facil.bmp",35,35,245,70);
    readimagefile("Imagenes\\medio.bmp",35,105,245,140);
    readimagefile("Imagenes\\dificil.bmp",35,175,245,210);
    readimagefile("Imagenes\\salir.bmp",35,280,245,315);
    while(opcion!=0)
    {

        posicionMouse(&mouse);
        if (GetAsyncKeyState(VK_LBUTTON))
            switch(mouse.x)
            {
            /// NIVEL FACIL
            case 1:
                grillaDim->y1 = grillaDim->x1 = 8;
                puntaje = 64;
                opcion=0;
                break;
            /// NIVEL MEDIO
            case 3:
                /// NIVEL DIFICIL
                grillaDim->y1 = grillaDim->x1 = 12;
                puntaje = 144;
                opcion=0;
                break;
            case 5:
                /// SALIDA
                grillaDim->y1 = grillaDim->x1 = 16;
                puntaje = 256;
                opcion=0;
                break;
            case 8:
                ShowWindow(GetForegroundWindow(),SW_SHOW);
                exit(1000);
            default:
                opcion=1;
            }
    }
    grillaDim->x2 = grilla * grillaDim->x1;
    grillaDim->y2 = grillaDim->y1 * grilla;
    closegraph();
    return puntaje;
}


/// COMPLETAR GRILLA CUANDO PIERDE
void perdiste(int **grillaMinas,int ** grillaMostrar,pixelesGrilla Dim)
{
    {
        int i,i2;
        Cordenadas aux;
        for (i2=0; i2<Dim.x1; i2++)
        {
            for (i=0; i<Dim.y1; i++)
            {
                aux.x = i2;
                aux.y = i;
                grillasBMP(aux,grillaMinas[i2][i]);
            }
        }
    }



}
/// FUNCION QUE USA LA DE MOSTRAR EL RADIO PARA CUADNO INICIA LA PARTIDA
void iniciarPrimerIntento(int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim)
{
    srand(time(NULL));
    Cordenadas mouseAux;
    do
    {
        mouseAux.x = rand()%grillaDim.x1;
        mouseAux.y = rand()%grillaDim.y1;
    }
    while (grillaMinas[mouseAux.x][mouseAux.y]!=0);
    botonIzquierdoGrilla(mouseAux,grillaMostrar,grillaMinas,grillaDim);

}

/// FUNCION PARA MOSTRAR EL RADIO DEL APRETADO
void radio(Cordenadas mouse,int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim)
{
    int aux;
    Cordenadas xy;
    for (int x = mouse.x-1; x<mouse.x+2 && x<grillaDim.x1; x++)
    {
        if (x==-1)
            x=0;
        xy.x = x;
        for (int y = mouse.y-1; y<mouse.y+2 && y<grillaDim.y1; y++)
        {
            aux = grillaMinas[x][y];
            if (grillaMinas[x][y]!=9 && grillaMostrar[x][y]!=20 && grillaMostrar[x][y]!=11)
            {
                xy.y = y;
                grillasBMP(xy,aux);
                grillaMostrar[x][y] = 20;
            }
        }

    }
}


/// LA FUNCION MAS DIFICL DE LOGRAR DE TODO EL PROGRAMA
int botonIzquierdoGrilla(Cordenadas mouse,int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim)
{
    int jugar=1;
    int aux=grillaMinas[mouse.x][mouse.y];
    Cordenadas auxMov;
    auxMov.y = mouse.y;
    auxMov.x = mouse.x;
    while (auxMov.y>0 && grillaMinas[auxMov.x][auxMov.y]==0)
    {
        while(auxMov.x<grillaDim.x1  && grillaMinas[auxMov.x][auxMov.y]==0)
        {
            radio(auxMov,grillaMostrar,grillaMinas,grillaDim);
            auxMov.x++;

        }
        auxMov.x = mouse.x;
        while(auxMov.x>0 && grillaMinas[auxMov.x][auxMov.y]==0)
        {
            radio(auxMov,grillaMostrar,grillaMinas,grillaDim);
            auxMov.x--;

        }
        auxMov.x = mouse.x;
        auxMov.y--;

    }
    auxMov.y = mouse.y;
    auxMov.x = mouse.x;
    while (auxMov.y<grillaDim.y1 && grillaMinas[auxMov.x][auxMov.y]==0)
    {
        while(auxMov.x<grillaDim.x1  && grillaMinas[auxMov.x][auxMov.y]==0)
        {
            radio(auxMov,grillaMostrar,grillaMinas,grillaDim);
            auxMov.x++;

        }
        auxMov.x = mouse.x;
        while(auxMov.x>0 && grillaMinas[auxMov.x][auxMov.y]==0)
        {
            radio(auxMov,grillaMostrar,grillaMinas,grillaDim);
            auxMov.x--;

        }
        auxMov.x = mouse.x;
        auxMov.y++;
    }
    if (aux!=0 && aux!=9)
    {
        grillasBMP(mouse,aux);
        grillaMostrar[mouse.x][mouse.y] = 20;
    }
    if (aux==9)
        jugar=0;

    return jugar;

}
/// CUENTA GRILLAS CONVERTIDAS EN FLAGS O MOSTRADOS
int contarPasados(int **grillaMostrar,int **grillaMinas,pixelesGrilla grillaDim)
{
    int contador=0;
    for (int x=0; x<grillaDim.x1; x++)
        for (int y=0; y<grillaDim.y1; y++)
        {
            if (grillaMostrar[x][y]==20 || grillaMostrar[x][y]==11)
                contador++;
        }
    return contador;
}
/// UTILIZO LA FUNCION QUE LLAMA A LAS GRILLAS CON NUMEROS PARA MOSTRAR LA CANTIDAD DE FLAGS USADOS
void contadorMinas(pixelesGrilla inferior,int contador)
{
    int a,b;
    //printf("CONTADOR %d",contador);
    b = contador % 10;
    contador/=10;
    a = contador % 10;
    if (a==0)
        a=100;
    if (b==9)
        b=99;
    if (b==0)
        b=100;
    Cordenadas aux1,aux2;
    aux2.x = inferior.x1+1;
    aux2.y = inferior.y1-1;
    aux1.x = inferior.x1+1;
    aux1.y = inferior.y1-2;
    grillasBMP(aux1,a);
    grillasBMP(aux2,b);
}
/// DIBUJA UNA GRILLA EN BASE A LA POSICION QUE SE LE ENVIA
void grillasBMP(Cordenadas mouse,int aux)
{
    pixelesGrilla auxiliarPixeles;
    auxiliarPixeles.x1 = mouse.x * grilla;
    auxiliarPixeles.x2 = (mouse.x+1) * grilla;
    auxiliarPixeles.y1 = mouse.y * grilla;
    auxiliarPixeles.y2 = (mouse.y+1) * grilla;
    switch (aux)
    {
    case 0:
        readimagefile("Imagenes\\grillaClicked.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 1:
        readimagefile("Imagenes\\grilla1.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 2:
        readimagefile("Imagenes\\grilla2.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 3:
        readimagefile("Imagenes\\grilla3.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 4:
        readimagefile("Imagenes\\grilla4.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 5:
        readimagefile("Imagenes\\grilla5.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 6:
        readimagefile("Imagenes\\grilla6.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 7:
        readimagefile("Imagenes\\grilla7.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 8:
        readimagefile("Imagenes\\grilla8.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 9:
        readimagefile("Imagenes\\grillaMine.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 99:
        readimagefile("Imagenes\\grilla9.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    case 100:
        readimagefile("Imagenes\\grilla0.bmp",auxiliarPixeles.y1,auxiliarPixeles.x1,auxiliarPixeles.y2,auxiliarPixeles.x2);
        break;
    }
}


/// FLAGS DE MINAS, CON UN CONTADOR QUE ++ O -- AL COLOCAR UNA
void grillaAflagMejorado(Cordenadas mouse,int **grillaMostrar,int *utnColocados)
{

    Cordenadas aux;
    aux.x = mouse.y * grilla;
    aux.y = mouse.x * grilla;
//    delay(175);
    if (grillaMostrar[mouse.x][mouse.y]==10 && (*utnColocados)>0)
    {
        readimagefile("Imagenes\\grillaFlag.bmp",aux.x,aux.y,aux.x+grilla,aux.y+grilla);
        grillaMostrar[mouse.x][mouse.y]=11;
        (*utnColocados)--;

    }
    else if(grillaMostrar[mouse.x][mouse.y]==11)
    {
        readimagefile("Imagenes\\grillaunidad.bmp",aux.x,aux.y,aux.x+grilla,aux.y+grilla);
        grillaMostrar[mouse.x][mouse.y]=10;
        (*utnColocados)++;
    }
}
/// OBTIENE LA POSICION DEL MOUSE CON EL MOVIMIENTO, DANDOLE UNA JUGABILIDAD MUY FLUIDA
void posicionMouse(Cordenadas *mouse)
{
    Cordenadas temporales;
    getmouseclick(WM_MOUSEMOVE,temporales.x,temporales.y);
    if (temporales.x!=-1 && temporales.y!=-1)
    {
        (*mouse).x = temporales.y / grilla;
        (*mouse).y = temporales.x / grilla;
    }
}

/// FUNCION MAGICA PARA CREAR LAS MATRICES DINAMICAMENTE ( CADA VEZ QUE LA VEO LA ENTIENDO MENOS )
void crearGrillaMatriz(int ***M,pixelesGrilla dim)
{
    int i=0;
    (*M)=(int**)calloc(dim.x1,sizeof(int *));
    for(i=0; i<dim.x1; i++)
    {
        (*M)[i]=(int *)calloc(dim.y1,sizeof(int));
    }
}
/// CARGA LA GRILLA MOSTRAR CON 10 Y LA MINAS CON 9 SI UN RANDOM DE 5 DA  0
int cargarGrillas(int **Minas,int **Mostrar,pixelesGrilla dim)
{
    srand(time(NULL));
    int minasCargadas=0;
    int aux = dim.x1 * dim.y1 * 0.15625;
    while (minasCargadas!=aux)
    {
        minasCargadas=0;
        for (int i=0; i<dim.x1; i++)
        {
            for (int j=0; j<dim.y1; j++)
            {
                Mostrar[i][j]=10;
                if (rand()%10==0)
                {
                    Minas[i][j]=9;
                    minasCargadas++;
                }
                else
                    Minas[i][j]=0;
            }
        }
    }

    return minasCargadas;
}
/// NUNCA ESTA DE MAS PARA SABER COMO VIENEN LAS COSAS
void mostrarGrillaCargada(int **Minas,pixelesGrilla dim)
{
    for (int i=0; i<dim.x1; i++)
    {
        printf("\n");
        for (int j=0; j<dim.y1; j++)
        {
            printf("%d ",Minas[i][j]);
        }

    }
}
/// PRESENTACION BIEN CHULA
void Presentacion(void)
{
    DWORD screenX = (GetSystemMetrics(SM_CXSCREEN)/2 )-350;
    DWORD screenY = (GetSystemMetrics(SM_CYSCREEN)/2)-177;
    int window = initwindow(540,410,"",screenX,screenY);
    bar(0,0,640,480);
    setcolor(BROWN);
    setfillstyle(SLASH_FILL, BLUE);
    bar(25,25,50,300);
    bar(50,300,125,275);
    bar(125,25,150,300);
    bar(250,50,275,300);
    bar(175,25,350,50);
    bar(375,25,400,300);
    bar(490,25,515,300);
    bar(400,25,410,55);
    bar(410,55,420,85);
    bar(420,85,430,115);
    bar(430,115,440,145);
    bar(440,145,450,175);
    bar(450,175,460,205);
    bar(460,205,470,235);
    bar(470,235,480,265);
    bar(480,255,490,300);

    settextstyle(BOLD_FONT,HORIZ_DIR,6);

    outtextxy(30,330," P R E S E N T A ");


    while (!GetAsyncKeyState(VK_LBUTTON))
    {
    }
    delay(500);
    closegraph();
}

/// COMPLETA LA MATRIZ MINAS CON LOS ADYACENTES SUMANDOLE UNA UNIDAD A CADA ESPACIO QUE TENGA UN 9 CERCA
void completarMatrizMinas(int **Minas,pixelesGrilla dim)
{
    int CompletarX=0,CompletarY=0;
    for (int i=0; i<dim.x1; i++)
        for (int j=0; j<dim.x1; j++)
        {
            if (Minas[i][j]==9)
            {
                for (CompletarX=i-1; CompletarX<i+2 && CompletarX!=dim.x1; CompletarX++)
                {
                    if (CompletarX==-1)
                        CompletarX=0;
                    for (CompletarY=j-1; CompletarY<j+2 && CompletarY!=dim.y1; CompletarY++)
                        if (Minas[CompletarX][CompletarY]!=9)
                            Minas[CompletarX][CompletarY]+=1;
                }
            }

        }

}

/// NO HAY MUCHO QUE DECIR, LA DIBUJA XD
void dibujarGrilla(int cantidadC, int cantidadF)
{
    int i,i2,x1,x2,y1,y2;
    for (i2=0; i2<cantidadF; i2++)
    {
        y1=grilla*i2;
        y2=grilla*(i2+1);
        for (i=0; i<cantidadC; i++)
        {
            x1=grilla*i;
            x2=grilla*(i+1);
            readimagefile("Imagenes\\grillaunidad.bmp",x1,y1,x2,y2);
        }
    }
}




/// DESPUES DE DIBUJAR LA GRILLA LE AGREGO UNA FILA MAS CON LOS DATOS DEL USUARIO
void dibujarParteInferior(pixelesGrilla inferior)
{
    int aux=0;
    for (int x=0; x<=grilla; x+=grilla)
    {
        for (int y=0; y<inferior.x1; y++)
        {
            aux = grilla * y;
            readimagefile("Imagenes\\grillaB&W.bmp",aux,inferior.y2+x,aux+grilla,inferior.y2+grilla+x);
            if (y==inferior.x1-3 && x==grilla)
            readimagefile("Imagenes\\grillaFlag.bmp",aux,inferior.y2+x,aux+grilla,inferior.y2+grilla+x);
        }
    }
}

/// MUSICA ADICTIVA
void musicaDJ(void)
{
//    PlaySound("sonidos\\loopmp.wav",NULL,SND_ASYNC | SND_LOOP);

}
