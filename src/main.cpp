#include <iostream>
#include "Engine/SDL_Wrapper.h"
#include "Engine/Game.h"
#include "Engine/TileSet/TileMap.h"
#include "TitleState.h"

using namespace std;
int main (int argc, char** argv) {
	try{
		Game *gm = new Game("30%", 2*600, 480);
		gm->Push(new TitleState());
		gm->Run();
	}catch(int e){
		switch(e){
		case WINDOW_FAIL:
			cout << "Erro ao abrir uma janela!" << endl;
			break;
		case RENDERER_FAIL:
			cout << "Erro ao iniciar um renderizador!" << endl;
			break;
		case IMG_LOAD_FAIL:
			cout << "Erro ao abrir uma imagem!" << endl;
			break;
		case INDEX_OUT_OF_RANGE:
			cout << "Indice invalido" << endl;
			break;
		case NO_INITIAL_STATE:
			cout << "Estado inicial nao definido" << endl;
			break;
		case MUSIC_LOAD_FAIL:
			cout << "Falha ao abrir uma musica" << endl;
			break;
		}
	}
    return 0;
}
