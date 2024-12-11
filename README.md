# WAV Audio Editor

## English

Audio Editor is an audio editor written in C++ using WinAPI. It provides a full set of features for editing, playing, and recording WAV files, supporting multi-channel audio. The program also includes the ability to visualize audio data, which can be edited directly in the graphical interface.

### Main Features

#### Audio Editing

- **Insert** audio at any position.
- **Delete** the selected part of the audio file.
- **Delete everything except the selected part**.
- **Copy** the selected area.
- **Undo/Redo**: undo or revert to the previous state.

#### Audio Data Visualization

- **Audio wave visualization**: displays audio data as a graph (waveform).
- **Edit through visualization**: you can edit the audio file by changing it directly on the graph, selecting and modifying sections of the visualized waveform.

#### Settings Adjustment

- **Speed adjustment**: from 0.01 to 100.00 with linear interpolation.
- **Encoding converter**:
  - Float
  - Unsigned int8
  - Signed int16
  - Signed int24
  - Signed int32
- **Sampling rate adjustment**: range from 8000 to 192000 Hz.

#### Player

- **Playback** of the audio file with applied settings.
- **Pause and resume** playback.
- **Jump to the beginning/end**.
- **Playback the selected area** starting from any position.
- **Playback from a specified start position**: you can choose the starting point for playback and listen to audio from that position.
- **Looping**: you can loop a selected area or playback from a specified start position to the end of the file, repeating it infinitely.
- **Support for 1 to 32 channels**.

#### Audio Recording

- **Recording** in mono or stereo mode (1-2 channels).
- **Select encoding and sampling rate**.
- **Pause and resume** during recording.
- **Stop recording**.
- **Simultaneous recording and playback**: you can record and play audio at the same time.
- **Real-time editing**: ability to edit the audio file while recording or playing it.

### Saving the File

You can save the modified file with any name and settings.

### Requirements

- Windows OS
- C++ compiler (e.g., Visual Studio)
- Minimum standart version required - ISO C++ 17. 

### Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/afanofrafa/WAV-Audio-Editor
2. Open the project in Visual Studio.
3. Build and run the project.

---

## Русский

**Audio Editor** — это аудиоредактор, написанный на C++ с использованием WinAPI. Программа предоставляет полный набор функций для редактирования, воспроизведения и записи WAV-файлов, поддерживая работу сразу над несколькими аудиоканалами. Также имеется возможность визуализации аудиоданных, которые могут редактироваться непосредственно в графическом интерфейсе.

### Основные функции

#### Редактирование аудио

- **Вставка** аудио в любую позицию.
- **Удаление** выделенной части звукового файла.
- **Удаление всего, кроме выделенной части**.
- **Копирование** выделенной области.
- **Undo/Redo**: отмена или возврат к предыдущему состоянию.

#### Визуализация аудиоданных

- **Визуализация аудиоволн**: отображение звуковых данных в виде графика (волновой формы).
- **Редактирование через визуализацию**: можно редактировать аудиофайл, изменяя его прямо на графике, выбирая и изменяя участки на визуализированной волне.

#### Изменение настроек

- **Изменение скорости**: от 0.01 до 100.00 с линейной интерполяцией.
- **Конвертер кодировок**:
  - Float
  - Unsigned int8
  - Signed int16
  - Signed int24
  - Signed int32
- **Изменение дискретизации**: диапазон от 8000 до 192000 Гц.

#### Плейер

- **Воспроизведение** звукового файла с учетом настроек.
- **Пауза и продолжение** воспроизведения.
- **Перемещение в начало/конец**.
- **Воспроизведение выделенной области** стартом с любой позиции.
- **Воспроизведение с указанной стартовой позиции**: можно выбрать стартовую точку для воспроизведения и слушать аудио с этой позиции.
- **Зацикливание**: можно зациклить выделенную область или воспроизведение с указанной стартовой позиции до конца файла, повторяя его бесконечно.
- **Поддержка от 1 до 32 каналов**.

#### Запись звука

- **Запись** в моно- или стереорежиме (1-2 канала).
- **Выбор кодировки и частоты дискретизации**.
- **Пауза и продолжение** во время записи.
- **Завершение записи**.
- **Одновременная запись и воспроизведение**: можно записывать звук и воспроизводить его одновременно.
- **Редактирование в реальном времени**: возможность редактировать звуковой файл прямо во время записи и воспроизведения.

### Сохранение файла

Изменённый файл можно сохранить с любым именем и параметрами.

### Требования

- Windows OS
- C++ компилятор (например, Visual Studio)
- Минимальный требуемый стандарт языка - ISO C++ 17.

### Установка

1. Склонируйте этот репозиторий:
   ```bash
   git clone https://github.com/afanofrafa/WAV-Audio-Editor
2. Откройте проект Visual Studio.
3. Соберите и запустите проект.
