#include "beep.h"

// 定义音符频率（可以根据需要添加更多）
#define NOTE_C4   262  // C4 音符频率为 262 Hz
#define NOTE_D4   294  // D4 音符频率为 294 Hz
#define NOTE_E4   330  // E4 音符频率为 330 Hz
#define NOTE_F4   349  // F4 音符频率为 349 Hz
#define NOTE_G4   392  // G4 音符频率为 392 Hz
#define NOTE_A4   440  // A4 音符频率为 440 Hz
#define NOTE_B4   494  // B4 音符频率为 494 Hz
#define NOTE_REST 0    // 休止符

// 定义音符长度（以毫秒为单位）
#define QUARTER_NOTE 200  // 四分音符长度为 200ms

// 音符序列（示例：简单的《小星星》主旋律）
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  NOTE_REST
};

// 对应音符长度
int noteDurations[] = {
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
  1000  // 休止符持续时间
};

void play()
{
	// 播放音符序列
    for (int i = 0; melody[i] != NOTE_REST; i++)
    {
			Set_Freqeucy_Cycle(melody[i]);
      HAL_Delay(noteDurations[i]);      // 按照音符长度延迟
    }
}