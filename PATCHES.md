# Adaptações do upstream

O código-base é o commit registrado em `upstream/UPSTREAM_COMMIT`.

- `upstream/g_game.c`: troca o cálculo de FPS de diagnóstico de ponto flutuante
  por aritmética inteira, porque os programas do SaguiOS são compilados sem FPU.
- `upstream/v_video.c`: desativa a caixa de velocidade do mouse, recurso fora do
  escopo deste port e que dependia de ponto flutuante.
- `upstream/i_system.c`: faz `I_Quit` encerrar o processo depois das funções de
  saída, para que o comando Quit do Doom feche a janela no SaguiOS.
- O build substitui `doomgeneric_soso.c`, `i_input.c` e `w_file_stdc.c` pelas
  implementações `doomgeneric_sagui.c`, `i_input_sagui.c` e `w_file_sagui.c`.
