# Pomodoro Timer

## Introdução
Este projeto implementa um Timer Pomodoro utilizando o Raspberry Pi Pico. O Timer Pomodoro é uma técnica de gerenciamento de tempo que alterna períodos de trabalho focado com pequenos intervalos de descanso.

## Requisitos de Hardware
- Raspberry Pi Pico
- Display OLED SSD1306
- Botões (3 unidades)
- LEDs (3 unidades: vermelho, azul e verde)
- Resistores

## Requisitos de Software
- [Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [CMake](https://cmake.org/)
- [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- [Visual Studio Code](https://code.visualstudio.com/) (opcional, mas recomendado)

## Instruções de Uso
1. Clone o repositório:
    ```sh
    git clone https://github.com/KayleKylian/Pomodoro-Timer.git
    cd Pomodoro-Timer
    ```

2. Configure o ambiente do Pico SDK:
    ```sh
    export PICO_SDK_PATH=/path/to/pico-sdk
    ```

3. Crie um diretório de build e compile o projeto:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

4. Carregue o firmware no Raspberry Pi Pico:
    - Conecte o Raspberry Pi Pico ao seu computador enquanto mantém pressionado o botão BOOTSEL.
    - Copie o arquivo `Pomodoro-Timer.uf2` gerado na pasta `build` para a unidade montada do Pico.

## Funcionamento
- **Botão A**: Inicia o Timer Pomodoro.
- **Botão B**: Pausa o Timer Pomodoro ou incrementa o tempo de trabalho se o timer não estiver em execução.
- **Botão Joystick**: Reseta o Timer Pomodoro ou incrementa o tempo de pausa se o timer não estiver em execução.

## Demonstração em Vídeo
[![Demonstração do Pomodoro Timer](https://img.youtube.com/vi/aV5t_Mg4Uwo/0.jpg)](https://youtu.be/aV5t_Mg4Uwo)

## Estrutura do Projeto
- `src/`: Código fonte do projeto.
- `inc/`: Arquivos de cabeçalho externos.
- `build/`: Diretório de build (gerado após a compilação).
- `CMakeLists.txt`: Arquivo de configuração do CMake.

## Licença
Este projeto está licenciado sob a licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.