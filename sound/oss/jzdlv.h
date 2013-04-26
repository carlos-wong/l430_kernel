/*
 *
 */

#ifndef _JZ_DLV_H_
#define _JZ_DLV_H_


#define DLV_MAX_RATE_COUNT	11

void dlv_write_reg(int addr, int val);
void dump_dlv_regs(const char *str);
/* header file for dlv */
static void write_dlv_reg(int addr, int val);
static int read_dlv_reg(int addr);
static int write_dlv_reg_bit(int addr, int bitval, int mask_bit);
//static void set_audio_data_replay(void);
static void dlv_set_replay();
//static void unset_audio_data_replay(void);
//static void set_record_mic_input_audio_without_playback(void);
static void dlv_set_record_mic(void);
//static void unset_record_mic_input_audio_without_playback(void);
//static void set_record_line_input_audio_without_playback(void);
static void dlv_set_record_line(void);
//static void unset_record_line_input_audio_without_playback(void);
static void set_playback_line_input_audio_direct_only(void);
//static void unset_playback_line_input_audio_direct_only(void);
//static void set_record_mic_input_audio_with_direct_playback(void);
static void dlv_set_record_mic_dir_playback(void);
//static void unset_record_mic_input_audio_with_direct_playback(void);
static void set_record_playing_audio_mixed_with_mic_input_audio(void);
//static void unset_record_playing_audio_mixed_with_mic_input_audio(void);
//static void set_record_mic_input_audio_with_audio_data_replay(void);
static void dlv_set_record_mic_with_replay(void);
//static void unset_record_mic_input_audio_with_audio_data_replay(void);
static void set_record_line_input_audio_with_audio_data_replay(void);
//static void unset_record_line_input_audio_with_audio_data_replay(void);
#endif /*_JZ_DLV_H_*/
