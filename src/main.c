#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ARVORES 30
#define MAX_FOGO 500
#define MAX_CENOURAS 10
#define MAX_FOLEGO 150
#define TEMPO_ARVORE_MIN 30
#define FPS 60.0
#define ESTADO_PRE_MENU 0
#define ESTADO_MENU 1
#define ESTADO_PRE_JOGO 2
#define ESTADO_JOGO 3
#define ESTADO_FIM 4
#define FIM_TUTORIAL 9

ALLEGRO_DISPLAY *janela;
ALLEGRO_BITMAP *buffer_janela;
ALLEGRO_BITMAP *fundo_arvores[4];
ALLEGRO_BITMAP *grama;
ALLEGRO_BITMAP *arbusto;
ALLEGRO_BITMAP *chao;
ALLEGRO_BITMAP *chao_vitoria;
ALLEGRO_BITMAP *canteiro;
ALLEGRO_BITMAP *sombra;
ALLEGRO_BITMAP *cenoura;
ALLEGRO_BITMAP *fogobitmap;
ALLEGRO_BITMAP *arvore;
ALLEGRO_BITMAP *barra;
ALLEGRO_BITMAP *icone_agua;
ALLEGRO_BITMAP *icone_fogo;
ALLEGRO_BITMAP *praia;
ALLEGRO_BITMAP *arvore_menu;
ALLEGRO_FONT *fonte_normal;
ALLEGRO_FONT *fonte_titulo;
ALLEGRO_FONT *fonte_contador;
ALLEGRO_FONT *fonte_pequena;
ALLEGRO_EVENT_QUEUE *fila_eventos;
ALLEGRO_TIMER *timer;
ALLEGRO_SAMPLE *som_come;
ALLEGRO_SAMPLE *som_corre;
ALLEGRO_SAMPLE *som_madeira;
ALLEGRO_AUDIO_STREAM *musica_fundo;
ALLEGRO_AUDIO_STREAM *musica_fogo;
ALLEGRO_SAMPLE_ID id_som;

int largura_tela, altura_tela;
int largura_janela, altura_janela;
float ratio_tela, tamanho_letterbox;
float ratio_x, ratio_y;
float dpi_scale=0;
int estado = 0;
int desenha=1;
float parallax_fundo[4]={0,0,0,0};
float parallax_chao=0;
float velocidade_mapa;
float movimento=0;
int cont_tempo=0;
int tempo_cenoura=60;
int tempo_dificuldade=FPS * 5;
float dificuldade=1;
float energia_cenoura=25;
float tempo_arvore=200;
int iarvores, narvores;
int ifogo, nfogo;
int icenouras, ncenouras;
float largura_arvore, altura_arvore, ratio_arvore;
float largura_cenoura, altura_cenoura;
int cenouras_comidas=0;
float pontos, distancia;
int dist_fogo;
int tempo_wave = 100;
int anda_wave = 0;
int dist_wave_max=100;
float tempo_percurso;
int morreu, venceu, esperando;
int mostra_creditos=0, mostra_tutorial=0;
int mostra_dificuldade=0, mostra_partida=0;
int imenu=0;
int janela_tutorial;


struct sprite {
    ALLEGRO_BITMAP *folha_sprite;
    int largura_sprite, altura_sprite;
    int regiao_x_folha, regiao_y_folha;
    int frames_sprite, cont_frames;
    int colunas_sprite, coluna_atual;
    int linhas_sprite, linha_atual;
    float pos_x, pos_y;
    float velocidade;
    float folego;
    int correndo;
    int anda, come, bate;
    int sprite_inicial, sprite_final;
};
struct sprite coelho;
struct sprite fogo[MAX_FOGO];
struct sprite fogo_intro;

struct objeto {
    ALLEGRO_BITMAP *folha_sprite;
    int largura_sprite, altura_sprite;
    int largura_hitbox, altura_hitbox;
    int coluna_atual, colunas_sprite;
    int regiao_x_folha, regiao_y_folha;
    int linha_atual, linhas_sprite;
    float pos_x, pos_y;
    float hitbox_x, hitbox_y;
    int vida;
};
struct objeto arvores[MAX_ARVORES];
struct objeto cenouras[MAX_CENOURAS];

void error_msg(char *text){
    if (janela)
        al_show_native_message_box(janela,"ERRO",
            "Ocorreu o seguinte erro e o programa sera finalizado:",
            text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
    else
        al_show_native_message_box(NULL,"ERRO",
            "Ocorreu o seguinte erro e o programa sera finalizado:",
            text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

void destroy(){
    if (janela)
        al_destroy_display(janela);
    if (buffer_janela)
        al_destroy_bitmap(buffer_janela);
    if (fundo_arvores[0])
        al_destroy_bitmap(fundo_arvores[0]);
    if (fundo_arvores[1])
        al_destroy_bitmap(fundo_arvores[1]);
    if (fundo_arvores[2])
        al_destroy_bitmap(fundo_arvores[2]);
    if (fundo_arvores[3])
        al_destroy_bitmap(fundo_arvores[3]);
    if (grama)
        al_destroy_bitmap(grama);
    if (arbusto)
        al_destroy_bitmap(arbusto);
    if (barra)
        al_destroy_bitmap(barra);
    if (cenoura)
        al_destroy_bitmap(cenoura);
    if (arvore)
        al_destroy_bitmap(arvore);
    if (icone_agua)
        al_destroy_bitmap(icone_agua);
    if (icone_fogo)
        al_destroy_bitmap(icone_fogo);
    if (coelho.folha_sprite)
        al_destroy_bitmap(coelho.folha_sprite);
    if (fogobitmap)
        al_destroy_bitmap(fogobitmap);
    if (praia)
        al_destroy_bitmap(praia);
    if (arvore_menu)
        al_destroy_bitmap(arvore_menu);
    if (fonte_normal)
        al_destroy_font(fonte_normal);
    if (fonte_pequena)
        al_destroy_font(fonte_pequena);
    if (fonte_contador)
        al_destroy_font(fonte_contador);
    if (fonte_titulo)
        al_destroy_font(fonte_titulo);
    if (fila_eventos)
        al_destroy_event_queue(fila_eventos);
    if (timer)
        al_destroy_timer(timer);
    if (som_come)
        al_destroy_sample(som_come);
    if (som_corre)
        al_destroy_sample(som_corre);
    if (som_madeira)
        al_destroy_sample(som_madeira);
    if (musica_fundo)
        al_destroy_audio_stream(musica_fundo);
    if (musica_fogo)
        al_destroy_audio_stream(musica_fogo);
}

int inicializar(){
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //seta tela para fullscreen
    janela = al_create_display(0, 0); //cria display. o tamanho vai ser automatico
    if(!janela) {
        error_msg("Falha ao criar janela");
        destroy();
        return 0;
    }
    al_set_window_title(janela, "Bunnyrun");
    largura_janela = al_get_display_width(janela); //captura tamanho da janela criada
    altura_janela = al_get_display_height(janela);

    largura_tela = 708; //tamanho da tela do jogo. independe da janela criada
    altura_tela = 400;
    ratio_tela = (float)largura_tela/altura_tela; //proporcao entre largura/altura da tela do jogo
    //caso aspect ratio do jogo para o monitor seja diferente, calcula tamanho da faixa preta
    tamanho_letterbox = altura_janela - largura_janela/ratio_tela;
    altura_janela = largura_janela / ratio_tela;
    ratio_x = (float)largura_tela/largura_janela;
    ratio_y = (float)altura_tela/altura_janela;
    dpi_scale=0;
    buffer_janela = al_create_bitmap(largura_tela, altura_tela); //buffer que seja escalado de acordo com o tamanho da janela
    if(!buffer_janela) {
        error_msg("Falha ao criar buffer da janela");
        destroy();
        return 0;
    }

    al_init_primitives_addon();

    if (!al_init_image_addon()){
        error_msg("Falha ao inicializar o addon de imagens");
        destroy();
        return 0;
    }

    fundo_arvores[0] = al_load_bitmap("img/parallax-forest-back-trees.png");
    fundo_arvores[1] = al_load_bitmap("img/parallax-forest-lights.png");
    fundo_arvores[2] = al_load_bitmap("img/parallax-forest-middle-trees.png");
    fundo_arvores[3] = al_load_bitmap("img/parallax-forest-front-trees.png");
    if(!fundo_arvores[0] || !fundo_arvores[1] || !fundo_arvores[2] || !fundo_arvores[3]) {
        error_msg("Falha ao carregar imagens parallax-forest");
        destroy();
        return 0;
    }

    grama = al_load_bitmap("img/grama.png");
    arbusto = al_load_bitmap("img/arbusto.png");
    if(!grama || !arbusto) {
        error_msg("Falha ao carregar imagem de fundo");
        destroy();
        return 0;
    }

    cenoura = al_load_bitmap("img/carrot.png");
    if(!cenoura) {
        error_msg("Falha ao carregar imagem carrot.png");
        destroy();
        return 0;
    }

    fogobitmap = al_load_bitmap("img/fire2.png");
    if(!fogobitmap) {
        error_msg("Falha ao carregar imagem fire.png");
        destroy();
        return 0;
    }

    praia = al_load_bitmap("img/beach.png");
    if(!cenoura) {
        error_msg("Falha ao carregar imagem beach.png");
        destroy();
        return 0;
    }

    barra = al_load_bitmap("img/barra.png");
    if(!barra) {
        error_msg("Falha ao carregar imagem barra.png");
        destroy();
        return 0;
    }

    arvore = al_load_bitmap("img/trees.png");
    if(!arvore) {
        error_msg("Falha ao carregar imagem trees.png");
        destroy();
        return 0;
    }

    arvore_menu = al_load_bitmap("img/arvore_menu.png");
    if(!arvore) {
        error_msg("Falha ao carregar imagem arvore_menu.png");
        destroy();
        return 0;
    }

    icone_agua = al_load_bitmap("img/water.png");
    icone_fogo = al_load_bitmap("img/fire.png");
    if(!icone_agua || !icone_fogo) {
        error_msg("Falha ao carregar icones");
        destroy();
        return 0;
    }

    al_init_font_addon();
    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar add-on allegro_ttf");
        destroy();
        return 0;
    }

    fonte_normal = al_load_font("font/atari.ttf", 16, 0);
    fonte_pequena = al_load_font("font/atari.ttf", 10, 0);
    fonte_contador = al_load_font("font/atari.ttf", 72, 0);
    if (!fonte_normal || !fonte_pequena || !fonte_contador){
        error_msg("Falha ao carregar fonte atari.ttf");
        destroy();
        return 0;
    }

    fonte_titulo = al_load_font("font/QumpellkaNo12.otf", 32, 0);
    if (!fonte_titulo){
        error_msg("Falha ao carregar fonte QumpellkaNo12.otf");
        destroy();
        return 0;
    }

    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        destroy();
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if(!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        destroy();
        return 0;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        error_msg("Falha ao criar temporizador");
        destroy();
        return 0;
    }

    if(!al_install_audio()){
        error_msg("Falha ao inicializar o audio");
        destroy();
        return 0;
    }

    if(!al_init_acodec_addon()){
        error_msg("Falha ao inicializar o codec de audio");
        destroy();
        return 0;
    }

    if (!al_reserve_samples(64)){
        error_msg("Falha ao reservar amostras de audio");
        destroy();
        return 0;
    }

    som_come = al_load_sample( "sound/eating.ogg" );
    som_corre = al_load_sample( "sound/running.ogg" );
    som_madeira = al_load_sample( "sound/bump.ogg" );
    if (!som_come || !som_corre || !som_madeira){
        error_msg( "Amostra de audio nao carregada" );
        destroy();
        return 0;
    }

    musica_fundo = al_load_audio_stream("sound/chase.ogg", 4, 1024);
    musica_fogo = al_load_audio_stream("sound/fire.ogg", 4, 1024);
    if (!musica_fundo || !musica_fogo){
        error_msg( "Audio stream nao carregado" );
        destroy();
        return 0;
    }

    al_attach_audio_stream_to_mixer(musica_fundo, al_get_default_mixer());
    al_attach_audio_stream_to_mixer(musica_fogo, al_get_default_mixer());
    al_set_audio_stream_playmode(musica_fundo, ALLEGRO_PLAYMODE_LOOP);
    al_set_audio_stream_playmode(musica_fogo, ALLEGRO_PLAYMODE_LOOP);
    al_set_audio_stream_playing(musica_fundo, 0);
    al_set_audio_stream_playing(musica_fogo, 0);

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_start_timer(timer);

    srand(time(NULL));

    sombra = al_create_bitmap(largura_tela, 5);
    chao = al_create_bitmap(largura_tela, altura_tela);
    canteiro = al_create_bitmap(largura_tela, al_get_bitmap_height(arbusto)*1.25);

    al_set_target_bitmap(chao);
    int i,j;
    int dif_x = 0;
    for (i=-al_get_bitmap_height(grama)/2 ; i<altura_tela ; i+=al_get_bitmap_height(grama)/2){
        if (!dif_x)
            dif_x = -al_get_bitmap_width(grama)/8;
        else
            dif_x = 0;
        for (j=dif_x; j<largura_tela ; j+=al_get_bitmap_width(grama)/4){
            al_draw_bitmap_region(grama, rand()%4 * al_get_bitmap_width(grama)/4, 0, al_get_bitmap_width(grama)/4, al_get_bitmap_height(grama), j, i, 0);
        }
    }

    chao_vitoria = al_clone_bitmap(chao);
    al_set_target_bitmap(chao_vitoria);
    al_draw_bitmap(praia, largura_tela - al_get_bitmap_width(praia), altura_tela - al_get_bitmap_height(praia), 0);

    int alpha=180;
    for (i=al_get_bitmap_height(fundo_arvores[0]) ; i<altura_tela ; i+=5){
        al_set_target_bitmap(sombra);
        al_clear_to_color(al_map_rgba(0, 0, 0, alpha));
        alpha-=3;
        al_set_target_bitmap(chao);
        al_draw_bitmap(sombra, 0, i, 0);
        al_set_target_bitmap(chao_vitoria);
        al_draw_bitmap(sombra, 0, i, 0);
    }
    al_set_target_bitmap(canteiro);
    for(i=-al_get_bitmap_width(arbusto) ; i<largura_janela ; i+=al_get_bitmap_width(arbusto)/10*4){
        al_draw_bitmap(arbusto, i, rand()%(al_get_bitmap_height(arbusto)/4), 0);
    }
    al_set_target_bitmap(al_get_backbuffer(janela));
    return 1;
}

void atualiza_parallax(){
    int i;
    for (i=0 ; i<4 ; i++){
        if (i!=1){
            if (!morreu && !venceu)
                parallax_fundo[i] -= (i+1)*0.3;
            if (parallax_fundo[i] < 0)
                parallax_fundo[i] += al_get_bitmap_width(fundo_arvores[i]);
        }
    }
}

void init_fogo(){
    if (nfogo < MAX_FOGO)
        nfogo++;
    ifogo = (ifogo+1)%nfogo;
    int espaco = al_get_bitmap_height(chao) - al_get_bitmap_height(canteiro)*2;
    fogo[ifogo].folha_sprite = fogobitmap;
    fogo[ifogo].largura_sprite = 128;
    fogo[ifogo].altura_sprite = 128;
    fogo[ifogo].colunas_sprite = 5;
    fogo[ifogo].linhas_sprite = 5;
    fogo[ifogo].coluna_atual = rand()%fogo[ifogo].colunas_sprite;
    fogo[ifogo].linha_atual = rand()%fogo[ifogo].linhas_sprite;
    fogo[ifogo].frames_sprite = 3;
    fogo[ifogo].cont_frames = 0;
    fogo[ifogo].pos_x = rand()%dist_fogo;
    fogo[ifogo].pos_y = rand()%espaco + al_get_bitmap_height(fundo_arvores[0]);
    fogo[ifogo].velocidade=10;
    fogo[ifogo].folego = 0;
    fogo[ifogo].anda = 0;
}

void init_coelho(){
    coelho.folha_sprite = al_load_bitmap("img/bunny_sheet.png");
    if (!coelho.folha_sprite){
        error_msg("Falha ao carregar sprite do coelho");
        destroy();
        return;
    }
    coelho.largura_sprite = 35;
    coelho.altura_sprite = 35;
    coelho.colunas_sprite = 3;
    coelho.linhas_sprite = 4;
    coelho.coluna_atual = 0;
    coelho.linha_atual = 0;
    coelho.frames_sprite = 6;
    coelho.cont_frames = 0;
    coelho.pos_x = 200;
    coelho.pos_y = 300;
    coelho.velocidade = 4;
    coelho.anda = 1;
    coelho.come = 0;
    coelho.bate = 0;
    coelho.correndo = 0;
    coelho.folego = MAX_FOLEGO;
    coelho.sprite_inicial = 0;
    coelho.sprite_final = 0;
}

void init_arvore(){
    if (narvores < MAX_ARVORES)
        narvores++;
    iarvores = (iarvores+1)%narvores;
    arvores[iarvores].folha_sprite = arvore;
    arvores[iarvores].colunas_sprite = 8;
    arvores[iarvores].linhas_sprite = 2;
    arvores[iarvores].coluna_atual = rand()%arvores[iarvores].colunas_sprite;
    arvores[iarvores].linha_atual = rand()%arvores[iarvores].linhas_sprite;
    arvores[iarvores].largura_sprite = 206;
    arvores[iarvores].altura_sprite = 300;
    arvores[iarvores].regiao_x_folha = arvores[iarvores].coluna_atual * arvores[iarvores].largura_sprite;
    arvores[iarvores].regiao_y_folha = arvores[iarvores].linha_atual * arvores[iarvores].altura_sprite;
    arvores[iarvores].largura_hitbox = 15 * ratio_arvore;
    arvores[iarvores].altura_hitbox = 20 * ratio_arvore;
    arvores[iarvores].hitbox_x = 95 * ratio_arvore;
    arvores[iarvores].hitbox_y = 250 * ratio_arvore;
    arvores[iarvores].pos_x = largura_tela;
    int min = al_get_bitmap_height(fundo_arvores[0]) + al_get_bitmap_height(canteiro)/2 - arvores[iarvores].hitbox_y - arvores[iarvores].altura_hitbox;
    int max = altura_tela - arvores[iarvores].hitbox_y - arvores[iarvores].altura_hitbox;
    arvores[iarvores].pos_y = rand()%(max-min+1)+min;
}

void init_cenoura(){
    if (ncenouras < MAX_CENOURAS)
        ncenouras++;
    icenouras = (icenouras+1)%ncenouras;

    cenouras[icenouras].folha_sprite = cenoura;
    cenouras[icenouras].regiao_x_folha = 0;
    cenouras[icenouras].regiao_y_folha = 0;
    cenouras[icenouras].largura_sprite = 23;
    cenouras[icenouras].altura_sprite = 32;
    cenouras[icenouras].pos_y = rand()%(altura_tela-(al_get_bitmap_height(fundo_arvores[0]) + al_get_bitmap_height(canteiro)/2))+(al_get_bitmap_height(fundo_arvores[0]) + al_get_bitmap_height(canteiro)/2 - altura_cenoura);
    cenouras[icenouras].pos_x = largura_tela - altura_cenoura;
    cenouras[icenouras].vida = 1;
}

void atualiza_fogo(){
    int i;
    for (i=0 ; i<nfogo ; i++){
        fogo[i].cont_frames++;
        if (fogo[i].cont_frames >= fogo[i].frames_sprite){
            fogo[i].cont_frames = 0;
            fogo[i].coluna_atual++;
            if (fogo[i].linha_atual*fogo[i].colunas_sprite + fogo[i].coluna_atual >= 40){
                fogo[i].linha_atual = 0;
                fogo[i].coluna_atual = 0;
            }
            else if (fogo[i].coluna_atual >= fogo[i].colunas_sprite){
                fogo[i].coluna_atual = 0;
                fogo[i].linha_atual++;
                if (fogo[i].linha_atual >= fogo[i].linhas_sprite){
                    fogo[i].linha_atual = 0;
                }
            }
            fogo[i].regiao_x_folha = fogo[i].largura_sprite * fogo[i].coluna_atual;
            fogo[i].regiao_y_folha = fogo[i].altura_sprite * fogo[i].linha_atual;
        }

        if (fogo[i].pos_x <= 0 && fogo[i].velocidade){
            fogo[i].velocidade=0;
            if (!venceu)
                init_fogo();
        }
        else{
            fogo[i].pos_x = fogo[i].pos_x - velocidade_mapa;
            fogo[i].folego += 10;
        }

        if (coelho.pos_x <= fogo[i].pos_x && fogo[i].folego >= 80 && !morreu){
            morreu=1;
        }

        if (venceu)
            fogo[i].pos_x -= 5;
        if (morreu){
            //init_fogo();
        }
    }
    if (!venceu && !morreu){
        if (cont_tempo%tempo_wave==0 && anda_wave == 0){
            anda_wave = 1;
            tempo_wave = rand()%700+300;
        }
        else if (dist_fogo < 50 && anda_wave == -1){
            anda_wave = 0;
        }
        else if (dist_fogo > dist_wave_max && anda_wave == 1){
            anda_wave = -1;
            dist_wave_max = rand()%150+100;
        }

        dist_fogo += 1 * anda_wave;
        al_set_audio_stream_gain(musica_fogo, 0.3 + 0.5 * (float)(dist_fogo)/(dist_wave_max));
    }

}

void atualiza_coelho(){
    int sprite_atual;
    coelho.cont_frames++;
    if (coelho.cont_frames >= coelho.frames_sprite){
        coelho.cont_frames = 0;

        if (coelho.anda){
            coelho.sprite_inicial = 0;
            coelho.sprite_final = 4;
        }
        if (coelho.come){
            coelho.sprite_inicial = 5;
            coelho.sprite_final = 7;
        }
        if (coelho.bate){
            coelho.sprite_inicial = 8;
            coelho.sprite_final = 10;
        }

        coelho.coluna_atual++;

        sprite_atual = coelho.coluna_atual + coelho.colunas_sprite * coelho.linha_atual;

        if (coelho.come && sprite_atual == coelho.sprite_final){
            coelho.come = 0;
            coelho.anda = 1;
        }
        if (coelho.bate && sprite_atual == coelho.sprite_final){
            coelho.bate = 0;
            coelho.anda = 1;
        }

        if (sprite_atual > coelho.sprite_final || sprite_atual < coelho.sprite_inicial){
            coelho.linha_atual = coelho.sprite_inicial / coelho.colunas_sprite;
            coelho.coluna_atual = coelho.sprite_inicial % coelho.colunas_sprite;
        }
        else if (coelho.coluna_atual >= coelho.colunas_sprite){
            coelho.coluna_atual = 0;
            coelho.linha_atual++;
            if (coelho.linha_atual >= coelho.linhas_sprite){
                coelho.linha_atual = 0;
            }
        }
        coelho.regiao_x_folha = coelho.largura_sprite * coelho.coluna_atual;
        coelho.regiao_y_folha = coelho.altura_sprite * coelho.linha_atual;

        if (coelho.bate){
            if (coelho.correndo)
                coelho.pos_x -= velocidade_mapa*5;
            else
                coelho.pos_x -= velocidade_mapa*3;
            coelho.pos_y += movimento * coelho.velocidade;
        }
        else{
            if (coelho.correndo && coelho.folego > 0){
                coelho.pos_x += coelho.velocidade*2 - velocidade_mapa;
                coelho.pos_y += movimento * coelho.velocidade*2;
                coelho.frames_sprite = 3;
                coelho.folego--;
            }
            else{
                coelho.pos_x += coelho.velocidade - velocidade_mapa;
                coelho.pos_y += movimento * coelho.velocidade;
                coelho.frames_sprite = 4;
            }
        }

        if (coelho.pos_y + coelho.altura_sprite > altura_tela)
            coelho.pos_y = altura_tela - coelho.altura_sprite;
        if (coelho.pos_y < al_get_bitmap_height(fundo_arvores[0]))
            coelho.pos_y = al_get_bitmap_height(fundo_arvores[0]);
        if (coelho.pos_x + coelho.largura_sprite >= largura_tela && !venceu)
            coelho.pos_x = largura_tela - coelho.largura_sprite;

    }
}

void atualiza_cenoura(){
    int i;
    for (i=0 ; i<ncenouras ; i++){
            cenouras[i].pos_x -= velocidade_mapa;
    }
}

void atualiza_arvore(){
    int i;
    for (i=0 ; i<narvores ; i++){
        arvores[i].pos_x -= velocidade_mapa;
    }
}

void colisao_cenoura(){
    float boca_x_coelho = coelho.pos_x + coelho.largura_sprite;
    float boca_y_coelho = coelho.pos_y + coelho.altura_sprite/2;
    int ok_x=0, ok_y=0, i;

    for (i=0 ; i<ncenouras ; i++){
        ok_x=0;
        ok_y=0;
        if (boca_y_coelho >= cenouras[i].pos_y && boca_y_coelho <= cenouras[i].pos_y+altura_cenoura)
            ok_y=1;
        if (boca_x_coelho >= cenouras[i].pos_x && boca_x_coelho <= cenouras[i].pos_x+altura_cenoura)
            ok_x=1;

        if (ok_x && ok_y && cenouras[i].vida > 0){
            coelho.folego+=energia_cenoura;
            if (coelho.folego > MAX_FOLEGO)
                coelho.folego = MAX_FOLEGO;
            coelho.come = 1;
            coelho.anda = 0;
            cenouras[i].vida = 0;
            cenouras_comidas++;
            al_play_sample(som_come, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    }
}

void colisao_arvore(){
    float cabeca_x_coelho = coelho.pos_x + coelho.largura_sprite;
    float cabeca_y_coelho = coelho.pos_y + coelho.altura_sprite/2;
    int i, ok_x, ok_y;
    for (i=0 ; i<narvores ; i++){
        ok_x=0;
        ok_y=0;
        if (cabeca_x_coelho >= arvores[i].pos_x + arvores[i].hitbox_x && cabeca_x_coelho <= arvores[i].pos_x + arvores[i].hitbox_x + arvores[i].largura_hitbox)
            ok_x = 1;
        if (cabeca_y_coelho >= arvores[i].pos_y + arvores[i].hitbox_y && cabeca_y_coelho <= arvores[i].pos_y + arvores[i].hitbox_y + arvores[i].altura_hitbox)
            ok_y = 1;
        if (ok_x && ok_y){
            if (!coelho.bate)
                al_play_sample(som_madeira, 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            coelho.bate = 1;
            return;
        }
    }
}

void desenha_fogo(){
    int i;
    for (i=0 ; i<nfogo ; i++){
        if (fogo[i].pos_x > 0){
            al_draw_scaled_bitmap(fogo[i].folha_sprite, fogo[i].regiao_x_folha, fogo[i].regiao_y_folha, fogo[i].largura_sprite, fogo[i].altura_sprite,
                              fogo[i].pos_x - fogo[i].largura_sprite/2 * fogo[i].folego/100,
                              fogo[i].pos_y - fogo[i].altura_sprite/2 * fogo[i].folego/100,
                              fogo[i].largura_sprite * fogo[i].folego/100,
                              fogo[i].altura_sprite * fogo[i].folego/100,
                              0);
        }
    }
}

void pre_menu(){
    mostra_tutorial=0;
    fogo_intro.folha_sprite = fogobitmap;
    fogo_intro.largura_sprite = 128;
    fogo_intro.altura_sprite = 128;
    fogo_intro.colunas_sprite = 5;
    fogo_intro.linhas_sprite = 5;
    fogo_intro.coluna_atual = 0;
    fogo_intro.linha_atual = 0;
    fogo_intro.frames_sprite = 3;
    fogo_intro.cont_frames = 0;
    fogo_intro.pos_x = largura_tela/2;
    fogo_intro.pos_y = altura_tela/2;

    al_stop_samples();
    al_set_audio_stream_playing(musica_fundo, 0);
    al_set_audio_stream_playing(musica_fogo, 1);
    al_set_audio_stream_gain(musica_fogo, 0.5);

    estado++;
}

void mostra_janela(int w, int h){
    al_draw_filled_rectangle(largura_tela/2 - w/2 - 5, altura_tela/2 - h/2 - 5, largura_tela/2 + w/2, altura_tela/2 + h/2, al_map_rgb(192,192,192));
    al_draw_filled_rectangle(largura_tela/2 - w/2, altura_tela/2 - h/2, largura_tela/2 + w/2 + 5, altura_tela/2 + h/2 + 5, al_map_rgb(64,64,64));
    al_draw_filled_rectangle(largura_tela/2 - w/2, altura_tela/2 - h/2, largura_tela/2 + w/2, altura_tela/2 + h/2, al_map_rgb(128,128,128));

    al_draw_filled_rounded_rectangle(largura_tela/2 + w/2 - al_get_text_width(fonte_pequena, "ESPACO") - 5,
                                     altura_tela/2 + h/2 - al_get_font_ascent(fonte_pequena) - 5,
                                     largura_tela/2 + w/2 - 1,
                                     altura_tela/2 + h/2 - 1, 2, 2, al_map_rgba(0,0,0,100));
    al_draw_textf(fonte_pequena, al_map_rgba(150,150,150,50),
                  largura_tela/2 + w/2 - al_get_text_width(fonte_pequena, "ESPACO") - 2,
                  altura_tela/2 + h/2 - al_get_font_ascent(fonte_pequena) - 3, ALLEGRO_ALIGN_LEFT, "ESPACO");
}

float ititulo=0, ivel=1;
void menu(){
    int i;
    int tam_menu = 5;
    char op_menu[5][50] = {"INICIAR","TUTORIAL","DIFICULDADE","CREDITOS","SAIR"}; //texto das opcoes do menu
    //calcula o espaco y para os items do menu
    float y_menu = 3 * altura_tela / (tam_menu+3);
    float x_menu = largura_tela/2;
    //quanto de espaco tem cada item
    float menu_inc = (altura_tela-y_menu)/tam_menu;
    ALLEGRO_COLOR cor_menu;
    ALLEGRO_EVENT evento;
    al_wait_for_event(fila_eventos, &evento);
    if(evento.type == ALLEGRO_EVENT_TIMER){
        fogo_intro.cont_frames++;
        if (fogo_intro.cont_frames >= fogo_intro.frames_sprite){
            fogo_intro.cont_frames = 0;
            fogo_intro.coluna_atual++;
            if (fogo_intro.linha_atual*fogo_intro.colunas_sprite + fogo_intro.coluna_atual >= 40){
                fogo_intro.linha_atual = 0;
                fogo_intro.coluna_atual = 0;
            }
            else if (fogo_intro.coluna_atual >= fogo_intro.colunas_sprite){
                fogo_intro.coluna_atual = 0;
                fogo_intro.linha_atual++;
                if (fogo_intro.linha_atual >= fogo_intro.linhas_sprite){
                    fogo_intro.linha_atual = 0;
                }
            }
            fogo_intro.regiao_x_folha = fogo_intro.largura_sprite * fogo_intro.coluna_atual;
            fogo_intro.regiao_y_folha = fogo_intro.altura_sprite * fogo_intro.linha_atual;
        }

        cont_tempo++;
        desenha = 1;
    }
    else if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
        if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE){
            if (mostra_creditos){ //se estiver mostrando creditos, manda sumir a caixa de texto
                mostra_creditos=0;
            }
            else if (mostra_tutorial){ //se estiver mostrando instrucoes, manda sumir a caixa de texto
                mostra_tutorial=0;
            }
            else if (mostra_dificuldade){
                mostra_dificuldade = 0;
            }
            else if (imenu == 0){ //inicia jogo
                estado++;
            }
            else if (imenu == 1){ //instrucoes
                mostra_tutorial=1;
                estado++;
            }
            else if (imenu == 2){
                mostra_dificuldade=1;
            }
            else if (imenu == 3){ //creditos
                mostra_creditos=1;
            }
            else if (imenu == 4){ //sair
                estado = ESTADO_FIM;
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_UP || evento.keyboard.keycode == ALLEGRO_KEY_W){
            if (mostra_dificuldade){
                if (dificuldade > 0)
                    dificuldade -= 0.5;
            }
            else if (!mostra_dificuldade && !mostra_creditos){
                imenu--; //sobe um item no menu
                if (imenu<0) //se apertar para cima no primeiro item, seleciona o ultimo
                    imenu = tam_menu-1;
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN || evento.keyboard.keycode == ALLEGRO_KEY_S){
            if (mostra_dificuldade){
                if (dificuldade < 2)
                    dificuldade += 0.5;
            }
            else if (!mostra_dificuldade && !mostra_creditos){
                imenu = (imenu+1)%tam_menu; //desce item no menu
            }
        }
    }
    else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        estado = ESTADO_FIM;
    }

    if(desenha && al_is_event_queue_empty(fila_eventos)) {
        al_set_target_bitmap(buffer_janela);
        al_clear_to_color(al_map_rgb(0,0,0));


        al_draw_scaled_bitmap(fogo_intro.folha_sprite, fogo_intro.regiao_x_folha, fogo_intro.regiao_y_folha, fogo_intro.largura_sprite, fogo_intro.altura_sprite,
                              -largura_tela*1.5, -altura_tela*1.5,
                              largura_tela*3 , altura_tela*3,
                              0);
        al_draw_scaled_bitmap(fogo_intro.folha_sprite, fogo_intro.regiao_x_folha, fogo_intro.regiao_y_folha, fogo_intro.largura_sprite, fogo_intro.altura_sprite,
                              -largura_tela*1.5 + largura_tela, -altura_tela*1.5,
                              largura_tela*3 , altura_tela*3,
                              0);

        //desenha retangulo marrom na tela
        float w = altura_tela * al_get_bitmap_width(arvore_menu)/al_get_bitmap_height(arvore_menu);
        al_draw_scaled_bitmap(arvore_menu, 0, 0, al_get_bitmap_width(arvore_menu), al_get_bitmap_height(arvore_menu),
                              largura_tela/2 - w/2, 0,
                              w , altura_tela,
                              0);
        //al_draw_filled_rectangle(200, 0, largura_tela-200, altura_tela, al_map_rgb(30,50,10));

        //a cada 100 frames, atualiza cor do titulo
        al_draw_filled_rounded_rectangle(x_menu - w/2 + 10, y_menu - menu_inc*2,
                                 x_menu + w/2 - 10, y_menu - menu_inc*2 + al_get_font_line_height(fonte_titulo) + 5,
                                 6, 6, al_map_rgba(0,0,0,70));
        if (ititulo >= 50 || ititulo < 0)
            ivel *= -1;
        ititulo+=ivel;
        ALLEGRO_COLOR cor_titulo = al_map_rgb(ititulo/50*135 + 120, ititulo/50*190 + 60, ititulo/50*60 + 30);
        al_draw_textf(fonte_titulo, cor_titulo, largura_tela/2, y_menu - menu_inc*2, ALLEGRO_ALIGN_CENTRE, "Fuga do Coelho");

        //desenha item do menu
        for (i=0 ; i<tam_menu ; i++){
            if (i == imenu) //desenha item de cor vermelha, caso seja o selecionado
                cor_menu = al_map_rgb(186,68,0);
            else
                cor_menu = al_map_rgb(170,255,160);
            al_draw_filled_rounded_rectangle(x_menu - al_get_text_width(fonte_normal, op_menu[i])/2 - 5, y_menu + i*menu_inc - 5,
                                     x_menu + al_get_text_width(fonte_normal, op_menu[i])/2 + 5, y_menu + i*menu_inc + al_get_font_ascent(fonte_normal) + 5,
                                     6, 6, al_map_rgba(0,0,0,70));
            al_draw_textf(fonte_normal, cor_menu, x_menu, y_menu + i*menu_inc, ALLEGRO_ALIGN_CENTRE, op_menu[i]);
        }

        //mostra a caixa com o texto dos creditos
        if (mostra_creditos){
            mostra_janela(500, 75);
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Desenvolvido por Pablo Werlang");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "pswerlang@gmail.com");
        }

        if (mostra_dificuldade){
            mostra_janela(150, 200);
            float w = fogo_intro.largura_sprite * (2-dificuldade -0.4 + 0.3 * dificuldade);
            float h = fogo_intro.altura_sprite * (2-dificuldade -0.4 + 0.3 * dificuldade);
            al_draw_scaled_bitmap(fogo_intro.folha_sprite, fogo_intro.regiao_x_folha, fogo_intro.regiao_y_folha, fogo_intro.largura_sprite, fogo_intro.altura_sprite,
                                  largura_tela/2 - w/2, altura_tela/2 - h/2,
                                  w, h,
                                  0);
            al_draw_filled_rounded_rectangle(largura_tela/2 - 10, altura_tela/2 - 100, largura_tela/2 + 8, altura_tela/2 - 100 + al_get_font_ascent(fonte_pequena) + 6, 3, 3, al_map_rgba(0,0,0,100));
            al_draw_filled_rounded_rectangle(largura_tela/2 - 10, altura_tela/2 + 100 - al_get_font_ascent(fonte_pequena) - 6, largura_tela/2 + 8, altura_tela/2 + 100, 3, 3, al_map_rgba(0,0,0,100));
            al_draw_textf(fonte_pequena, al_map_rgb(255,255,255), largura_tela/2, altura_tela/2 - 100 + 3, ALLEGRO_ALIGN_CENTRE, "W");
            al_draw_textf(fonte_pequena, al_map_rgb(255,255,255), largura_tela/2, altura_tela/2 + 100 - al_get_font_ascent(fonte_pequena) - 3, ALLEGRO_ALIGN_CENTRE, "S");
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_clear_to_color(al_map_rgb(0,0,0));
        //desenha o buffer na tela, escalando o tamanho
        al_draw_scaled_bitmap(buffer_janela, 0, 0, largura_tela, altura_tela, 0, tamanho_letterbox/2, largura_janela, altura_janela, 0);
        al_flip_display();
        desenha = 0;
    }
}

int pausa=0;
void verifica_tutorial(){
    if (mostra_tutorial==1 && cont_tempo > 20){
        float w = 50;
        float h = al_get_bitmap_height(fundo_arvores[0]);
        al_draw_filled_rectangle(0, 0, largura_tela, h, al_map_rgba(0,0,0,230));
        al_draw_filled_rectangle(w, h, largura_tela, altura_tela, al_map_rgba(0,0,0,230));
        mostra_janela(500, 150);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 45, ALLEGRO_ALIGN_CENTRE, "Ajude o coelho a fugir da");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "floresta em chamas.");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "Caso o fogo lhe alcance");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "sera o fim do jogo.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==2){
        mostra_janela(500, 150);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 45, ALLEGRO_ALIGN_CENTRE, "Utilize as tecla W/S para");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "se mover para cima/baixo");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "e a barra de espaco para correr");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "mais rapido.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==3 && coelho.folego < 120){
        float w = al_get_bitmap_width(barra);
        float h = al_get_bitmap_height(barra)/2;
        al_draw_filled_rectangle(0, 10 + h, largura_tela, altura_tela, al_map_rgba(0,0,0,230));
        al_draw_filled_rectangle(10 + w, 0, largura_tela, 10 + h, al_map_rgba(0,0,0,230));
        mostra_janela(500, 100);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Cuidado com o folego! Voce");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "precisa dele para correr rapido.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==4){
        mostra_janela(500, 150);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 45, ALLEGRO_ALIGN_CENTRE, "Colete as cenouras no chao");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "para recuperar uma porcao");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "do seu folego e aumentar");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "seus pontos.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==5 && coelho.come){
        mostra_janela(500, 120);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 30, ALLEGRO_ALIGN_CENTRE, "Delicioso!");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 5, ALLEGRO_ALIGN_CENTRE, "Agora corra um pouco");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 20, ALLEGRO_ALIGN_CENTRE, "para gastar essa energia.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==6 && coelho.bate){
        mostra_janela(500, 150);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 45, ALLEGRO_ALIGN_CENTRE, "Cuidado com as arvores!");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Esbarrar em uma ira lhe atrasar.");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "O efeito e ainda pior se voce");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "estiver correndo.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==7 && cont_tempo/tempo_percurso > 0.7){
        float w = largura_tela - 225;
        float h = altura_tela - 55;
        al_draw_filled_rectangle(0, 0, w, altura_tela, al_map_rgba(0,0,0,230));
        al_draw_filled_rectangle(w, 0, largura_tela, h, al_map_rgba(0,0,0,230));
        mostra_janela(500, 150);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 45, ALLEGRO_ALIGN_CENTRE, "Ali em baixo voce encontra");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "seu progresso no jogo.");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "Alcance a outra extremidade");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "e voce esta a salvo do incendio.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==8 && cont_tempo/tempo_percurso > 0.9){
        mostra_janela(450, 100);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Pressione ESC a qualquer");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "momento para pausar o jogo.");
        janela_tutorial=1;
        pausa=1;
    }
    else if (mostra_tutorial==9 && venceu > 300){
        mostra_janela(450, 100);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "PARABENS!");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "Voce completou o tutorial.");
        esperando=1;
    }
    else if (mostra_tutorial && morreu > 300){
        mostra_janela(450, 100);
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Voce nao conseguiu completar");
        al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "o tutorial. Tente novamente.");
        esperando=1;
    }
}

void pre_jogo(){
    cont_tempo = 1;
    dist_fogo = 50;
    nfogo=0;
    ifogo=0;
    ncenouras=0;
    icenouras=0;
    narvores=0;
    iarvores=0;
    morreu=0;
    venceu=0;
    velocidade_mapa=5;
    tempo_arvore = 200;
    esperando=0;
    pausa=0;
    cenouras_comidas=0;
    janela_tutorial=0;
    mostra_partida=1;

    init_coelho();

    init_arvore();
    largura_arvore = 400;
    altura_arvore = largura_arvore * (float)arvores[0].altura_sprite/arvores[0].largura_sprite;
    ratio_arvore = largura_arvore / arvores[0].largura_sprite;
    narvores=0;
    iarvores=0;

    init_cenoura();
    altura_cenoura = 30;
    largura_cenoura = altura_cenoura * (float)cenouras[0].largura_sprite/cenouras[0].altura_sprite;

    int i;
    for (i=0 ; i<50 ; i++)
        init_fogo();

    al_stop_samples();
    al_play_sample(som_corre, 0.8, 0.0, 0.7, ALLEGRO_PLAYMODE_LOOP, &id_som);


    float tempo_arvore_temp = tempo_arvore;
    float velocidade_mapa_temp = velocidade_mapa;
    float temp_dif = dificuldade;
    if (mostra_tutorial)
        temp_dif = 2.5;
    for (i=0 ; velocidade_mapa_temp < coelho.velocidade*2 - temp_dif ; i++){
        if (tempo_arvore_temp>TEMPO_ARVORE_MIN){
            tempo_arvore_temp *= 0.9;
            velocidade_mapa_temp+=0.075;
        }
        else
            velocidade_mapa_temp+=0.02;
    }

    tempo_percurso = i*tempo_dificuldade;

    al_stop_samples();
    al_set_audio_stream_playing(musica_fundo, 1);
    al_set_audio_stream_gain(musica_fundo, 0.7);
    al_set_audio_stream_playing(musica_fogo, 1);
    al_set_audio_stream_gain(musica_fogo, 0.3);

    estado++;
}

void jogo(){
    int i,j;
    ALLEGRO_EVENT evento;
    al_wait_for_event(fila_eventos, &evento);

    if(evento.type == ALLEGRO_EVENT_TIMER){
        if (mostra_partida){
            mostra_partida++;
            if ((int)(mostra_partida/FPS)==3){
                mostra_partida=0;
            }
        }
        else if (!pausa){
            atualiza_parallax();
            atualiza_coelho();
            atualiza_cenoura();
            colisao_cenoura();
            atualiza_arvore();
            colisao_arvore();
            atualiza_fogo();
            if (!morreu && !venceu){
                if (cont_tempo%(int)tempo_arvore == 0){
                    init_arvore();
                }
                if (cont_tempo%tempo_cenoura == 0)
                    init_cenoura();

                if (cont_tempo%tempo_dificuldade == 0){
                    if (tempo_arvore>TEMPO_ARVORE_MIN){
                        tempo_arvore *= 0.9;
                        velocidade_mapa+=0.075;
                    }
                    else
                        velocidade_mapa+=0.02;
                }

                if (cont_tempo >= tempo_percurso){
                    if (mostra_tutorial < FIM_TUTORIAL && mostra_tutorial != 0)
                        tempo_percurso++;
                    else
                        venceu=1;
                }

                cont_tempo++;
            }
            if (venceu){
                if (parallax_chao <= 10){
                    coelho.velocidade += velocidade_mapa;
                    velocidade_mapa = 0;
                }
                dist_fogo--;
                venceu++;
                al_set_audio_stream_gain(musica_fogo, (250-venceu)/250);
                if (coelho.pos_x > largura_tela){
                    al_stop_samples();
                    al_set_audio_stream_playing(musica_fogo, 0);
                    coelho.velocidade = 0;
                }
            }

            if (morreu){
                al_stop_samples();
                coelho.velocidade = 0;
                velocidade_mapa = 0;
                dist_fogo++;
                morreu++;
            }

            parallax_chao-=velocidade_mapa;
            if (parallax_chao < 0)
                parallax_chao=largura_tela;

            if (!morreu && !venceu){
                pontos = (cont_tempo / FPS)*cenouras_comidas;
                distancia = (cont_tempo / FPS) * 0.015;
            }
        }
        desenha=1;

    }
    else if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
        if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
            if (pausa)
                estado = ESTADO_PRE_MENU;
            else
                pausa = 1;
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE){
            if (janela_tutorial){
                janela_tutorial=0;
                pausa=0;
                if (mostra_tutorial)
                    mostra_tutorial++;
            }
            else if (pausa){
                pausa = 0;
            }
            else if (esperando){
                estado = ESTADO_PRE_MENU;
            }
            else if (!venceu){
                coelho.correndo = 1;
                al_stop_sample(&id_som);
                al_play_sample(som_corre, 0.8, 0.0, 0.9, ALLEGRO_PLAYMODE_LOOP, &id_som);
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_UP || evento.keyboard.keycode == ALLEGRO_KEY_W){
            movimento = -1;
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN || evento.keyboard.keycode == ALLEGRO_KEY_S){
            movimento = 1;
        }
    }
    else if (evento.type == ALLEGRO_EVENT_KEY_UP){
        if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE){
            coelho.correndo = 0;
            al_stop_sample(&id_som);
            al_play_sample(som_corre, 0.8, 0.0, 0.7, ALLEGRO_PLAYMODE_LOOP, &id_som);
        }
        else{
            movimento = 0;
        }
    }
    else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        estado = ESTADO_FIM;
    }

    if(desenha && al_is_event_queue_empty(fila_eventos)) {
        al_set_target_bitmap(buffer_janela);
        al_clear_to_color(al_map_rgb(0,0,0));

        if (venceu)
            al_draw_bitmap(chao_vitoria,parallax_chao,0,0);
        else
            al_draw_bitmap(chao,parallax_chao,0,0);
        al_draw_bitmap(chao,parallax_chao-largura_tela,0,0);
        for (j=0 ; j<4 ; j++){
            for (i=-al_get_bitmap_width(fundo_arvores[j]) ; i<largura_tela ; i+=al_get_bitmap_width(fundo_arvores[j])){
                al_draw_bitmap(fundo_arvores[j],i + parallax_fundo[j],0,0);
            }
        }
        al_draw_bitmap(canteiro,parallax_chao,al_get_bitmap_height(fundo_arvores[0])-al_get_bitmap_height(canteiro)/2,0);
        al_draw_bitmap(canteiro,parallax_chao-largura_tela,al_get_bitmap_height(fundo_arvores[0])-al_get_bitmap_height(canteiro)/2,0);

        for (i=0 ; i<ncenouras ; i++){
            if (cenouras[i].vida > 0)
                al_draw_scaled_bitmap(cenouras[i].folha_sprite, 0, 0, cenouras[i].largura_sprite, cenouras[i].altura_sprite,
                                              cenouras[i].pos_x, cenouras[i].pos_y,
                                              largura_cenoura, altura_cenoura,
                                              0);
        }

        for (i=0 ; i<narvores ; i++){
            if (arvores[i].pos_y + altura_arvore - altura_arvore*0.1 < coelho.pos_y + coelho.altura_sprite)
                al_draw_scaled_bitmap(arvores[i].folha_sprite, arvores[i].regiao_x_folha, arvores[i].regiao_y_folha, arvores[i].largura_sprite, arvores[i].altura_sprite, arvores[i].pos_x, arvores[i].pos_y, largura_arvore, altura_arvore, 0);
        }

        al_draw_bitmap_region(coelho.folha_sprite, coelho.regiao_x_folha, coelho.regiao_y_folha, coelho.largura_sprite, coelho.altura_sprite, coelho.pos_x, coelho.pos_y, 0);

        //desenha arvores
        for (i=0 ; i<narvores ; i++){
            if (arvores[i].pos_y + altura_arvore - altura_arvore*0.1 >= coelho.pos_y + coelho.altura_sprite)
                al_draw_scaled_bitmap(arvores[i].folha_sprite, arvores[i].regiao_x_folha, arvores[i].regiao_y_folha, arvores[i].largura_sprite, arvores[i].altura_sprite, arvores[i].pos_x, arvores[i].pos_y, largura_arvore, altura_arvore, 0);
        }

        desenha_fogo();

        //al_draw_textf(fonte, al_map_rgb(255,255,255), 20, 80, ALLEGRO_ALIGN_LEFT, "%.4f", cont_tempo/tempo_percurso);
        //al_draw_textf(fonte, al_map_rgb(255,255,255), 20, 100, ALLEGRO_ALIGN_LEFT, "%.4f", velocidade_mapa);

        float w = al_get_bitmap_width(barra);
        float h = al_get_bitmap_height(barra)/2;

        al_draw_filled_rounded_rectangle(0, 0, 10 + w, 10 + 15 + h, 3, 3, al_map_rgba(0,25,0,100));
        al_draw_bitmap_region(barra, 0, 0, al_get_bitmap_width(barra), al_get_bitmap_height(barra)/2, 5, 5, 0);
        al_draw_bitmap_region(barra, 0, al_get_bitmap_height(barra)/2, (float)coelho.folego/MAX_FOLEGO * al_get_bitmap_width(barra), al_get_bitmap_height(barra)/2, 5, 5, 0);
        al_draw_textf(fonte_normal, al_map_rgb(255,0,0), (10 + w)/2, 35, ALLEGRO_ALIGN_CENTRE, "PONTOS: %.0f", pontos);

        w = 200;
        h = 40;

        //progresso da corrida
        al_draw_filled_rounded_rectangle(largura_tela - 225, altura_tela - 55, largura_tela - 225 + w, altura_tela - 55 + h, 3, 3, al_map_rgba(0,25,0,100));
        al_draw_line(largura_tela - 225 + 15, altura_tela - 55 + 15, largura_tela - 225 + 15 + (w-30)*cont_tempo/tempo_percurso, altura_tela - 55 + 15, al_map_rgb(255,0,0), 2);
        al_draw_line(largura_tela - 225 + 15 + (w-30)*cont_tempo/tempo_percurso, altura_tela - 55 + 15, largura_tela - 225 + w - 15, altura_tela - 55 + 15, al_map_rgb(0,255,0), 2);
        al_draw_bitmap(icone_fogo, largura_tela - 225 + 15 - al_get_bitmap_width(icone_fogo)/2, altura_tela - 55 + 15 - al_get_bitmap_height(icone_fogo)/2, 0);
        al_draw_bitmap(icone_agua, largura_tela - 225 + w - 15 - al_get_bitmap_width(icone_agua)/2, altura_tela - 55 + 15 - al_get_bitmap_height(icone_agua)/2, 0);
        al_draw_filled_circle(largura_tela - 225 + 15 + (w-30)*cont_tempo/tempo_percurso, altura_tela - 55 + 15, 4, al_map_rgb(255,255,255));
        if (distancia < 1)
            al_draw_textf(fonte_normal, al_map_rgb(255,0,0), largura_tela - 225 + w/2, altura_tela - 55 + 20, ALLEGRO_ALIGN_CENTER, "%.0fm", distancia*1000);
        else
            al_draw_textf(fonte_normal, al_map_rgb(255,0,0), largura_tela - 225 + w/2, altura_tela - 55 + 20, ALLEGRO_ALIGN_CENTER, "%.2fKm", distancia);

        if(morreu>300 && !mostra_tutorial){
            mostra_janela(460, 120);
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 40, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 10, ALLEGRO_ALIGN_CENTRE, "Voce percorreu %.1fKm",distancia);
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 15, ALLEGRO_ALIGN_CENTRE, "e obteve %.0f pontos. ", pontos);
            esperando=1;
        }

        if(venceu>300 && !mostra_tutorial){
            mostra_janela(450, 150);
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 50, ALLEGRO_ALIGN_CENTRE, "PARABENS!");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Voce chegou em um local");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "seguro e terminou o jogo");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "com %.0f pontos.", pontos);
            esperando=1;
        }

        if (pausa && !janela_tutorial){
            mostra_janela(450, 150);
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 50, ALLEGRO_ALIGN_CENTRE, "PAUSE");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 - 20, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para voltar");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 5, ALLEGRO_ALIGN_CENTRE, "ao menu inicial, ou ESPACO");
            al_draw_textf(fonte_normal, al_map_rgb(0,0,0), largura_tela/2, altura_tela/2 + 30, ALLEGRO_ALIGN_CENTRE, "para voltar ao jogo");
        }

        if (mostra_tutorial)
            verifica_tutorial();

        if (mostra_partida){
            al_draw_textf(fonte_contador, al_map_rgb(255,0,0), largura_tela/2, altura_tela/2, ALLEGRO_ALIGN_CENTRE, "%i",3-(int)(mostra_partida/FPS));
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_scaled_bitmap(buffer_janela, 0, 0, largura_tela, altura_tela, 0, tamanho_letterbox/2, largura_janela, altura_janela, 0);
        al_flip_display();

        desenha = 0;
    }

}

int main(){
    if (!inicializar())
        return -1;

    while(estado != ESTADO_FIM){ //enquanto nao chegar no estado do fim de jogo, entra no estado adequado
        if (estado == ESTADO_PRE_MENU)
            pre_menu();
        else if (estado == ESTADO_MENU)
            menu();
        else if (estado == ESTADO_PRE_JOGO)
            pre_jogo();
        else if (estado == ESTADO_JOGO)
            jogo();
    }

    destroy();
    return 0;
}
