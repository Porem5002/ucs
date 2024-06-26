#ifndef UI_LABELS_HEADER
#define UI_LABELS_HEADER

#define PT_MENUS 0

#define UI_EDITOR_LABEL_8X8 "8x8"
#define UI_EDITOR_LABEL_10X10 "10x10"
#define UI_EDITOR_LABEL_12X12 "12x12"

#define UI_MENU_BUTTON_TEXT "MENU"
#define UI_SELECTOR_EDITOR_LABEL "EDITOR"

#if !PT_MENUS

#define UI_MESSAGE_CHALLENGE_COMPLETED "Challenge Completed"
#define UI_MESSAGE_BLACK_TEAM_WON      "Black Team Won"
#define UI_MESSAGE_WHITE_TEAM_WON      "White Team Won"

#define UI_START_BUTTON_TEXT "START"
#define UI_EDITOR_BUTTON_TEXT "EDITOR"
#define UI_QUIT_BUTTON_TEXT "QUIT"
#define UI_BACK_BUTTON_TEXT "BACK"

#define UI_SELECTOR_STANDARD_LABEL "STANDARD"

#define UI_EDITOR_LABEL_MAIN "MAIN"
#define UI_EDITOR_LABEL_RULES "RULES"
#define UI_EDITOR_LABEL_PIECES "PIECES"

#define UI_EDITOR_LABEL_YES "YES"
#define UI_EDITOR_LABEL_NO "NO"

#define UI_EDITOR_LABEL_RIGHT "RIGHT"
#define UI_EDITOR_LABEL_LEFT "LEFT"
#define UI_EDITOR_LABEL_SAVE "SAVE"

#define UI_EDITOR_LABEL_NAME "NAME"
#define UI_EDITOR_LABEL_BOARD "BOARD"
#define UI_EDITOR_LABEL_D_CORNER "DOUBLE CORNER"

#define UI_EDITOR_LABEL_START_PLAYER "STARTING PLAYER"
#define UI_EDITOR_LABEL_LAW_QUANTITY "LAW OF QUANTITY"
#define UI_EDITOR_LABEL_LAW_QUALITY "LAW OF QUALITY"
#define UI_EDITOR_LABEL_PEON_BACK_CAPTURE "PEONS CAPTURE BACKWARDS"
#define UI_EDITOR_LABEL_FLYING_KINGS "FLYING KINGS"

#else

#define UI_MESSAGE_CHALLENGE_COMPLETED  "Desafio Concluido"
#define UI_MESSAGE_BLACK_TEAM_WON       "Negras Venceram"
#define UI_MESSAGE_WHITE_TEAM_WON       "Brancas Venceram"

#define UI_START_BUTTON_TEXT "JOGAR"
#define UI_EDITOR_BUTTON_TEXT "EDITOR"
#define UI_QUIT_BUTTON_TEXT "SAIR"
#define UI_BACK_BUTTON_TEXT "VOLTAR"

#define UI_SELECTOR_STANDARD_LABEL "PRINCIPAL"

#define UI_EDITOR_LABEL_MAIN UI_SELECTOR_STANDARD_LABEL
#define UI_EDITOR_LABEL_RULES "REGRAS"
#define UI_EDITOR_LABEL_PIECES "PEÇAS"

#define UI_EDITOR_LABEL_YES "SIM"
#define UI_EDITOR_LABEL_NO "NÃO"

#define UI_EDITOR_LABEL_RIGHT "DIREITO"
#define UI_EDITOR_LABEL_LEFT "ESQUERDO"
#define UI_EDITOR_LABEL_SAVE "GUARDAR"

#define UI_EDITOR_LABEL_NAME "NOME"
#define UI_EDITOR_LABEL_BOARD "TABULEIRO"
#define UI_EDITOR_LABEL_D_CORNER "CANTO DUPLO"

#define UI_EDITOR_LABEL_START_PLAYER "JOGADOR INICIAL"
#define UI_EDITOR_LABEL_LAW_QUANTITY "LEI DA QUANTIDADE"
#define UI_EDITOR_LABEL_LAW_QUALITY "LEI DA QUALIDADE"
#define UI_EDITOR_LABEL_PEON_BACK_CAPTURE "PEÃO CAPTURA PARA TRÁS"
#define UI_EDITOR_LABEL_FLYING_KINGS "DAMAS VOADORAS"

#endif

#endif