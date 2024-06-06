#include "lv_anim_light.h"
#include "main.h"

int32_t lv_map(int32_t x, int32_t min_in, int32_t max_in, int32_t min_out, int32_t max_out)
{
  if(max_in >= min_in && x >= max_in) return max_out;
  if(max_in >= min_in && x <= min_in) return min_out;

  if(max_in <= min_in && x <= max_in) return max_out;
  if(max_in <= min_in && x >= min_in) return min_out;

  /**
   * The equation should be:
   *   ((x - min_in) * delta_out) / delta in) + min_out
   * To avoid rounding error reorder the operations:
   *   (x - min_in) * (delta_out / delta_min) + min_out
   */

  int32_t delta_in = max_in - min_in;
  int32_t delta_out = max_out - min_out;

  return ((x - min_in) * delta_out) / delta_in + min_out;
}

uint32_t lv_bezier3(uint32_t t, uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3)
{
  uint32_t t_rem  = 1024 - t;
  uint32_t t_rem2 = (t_rem * t_rem) >> 10;
  uint32_t t_rem3 = (t_rem2 * t_rem) >> 10;
  uint32_t t2     = (t * t) >> 10;
  uint32_t t3     = (t2 * t) >> 10;

  uint32_t v1 = (t_rem3 * u0) >> 10;
  uint32_t v2 = (3 * t_rem2 * t * u1) >> 20;
  uint32_t v3 = (3 * t_rem * t2 * u2) >> 20;
  uint32_t v4 = (t3 * u3) >> 10;

  return v1 + v2 + v3 + v4;
}

int32_t lv_anim_path_ease_out(const lv_anim_t* a)
{
  /*Calculate the current step*/
  uint32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
  int32_t step = lv_bezier3(t, 0, 900, 950, LV_BEZIER_VAL_MAX);

  int32_t new_value;
  new_value = step * (a->end_value - a->start_value);
  new_value = new_value >> LV_BEZIER_VAL_SHIFT;
  new_value += a->start_value;

  return new_value;
}
static uint32_t last_timer_run;
uint32_t lv_tick_elaps(uint32_t prev_tick)
{
  uint32_t act_time = HAL_GetTick();

  /*If there is no overflow in sys_time simple subtract*/
  if(act_time >= prev_tick)
  {
    prev_tick = act_time - prev_tick;
  }
  else
  {
    prev_tick = UINT32_MAX - prev_tick + 1;
    prev_tick += act_time;
  }

  return prev_tick;
}

void lv_anim_init(lv_anim_t* a)
{
  a->time = 500;
  a->start_value = 0;
  a->end_value = 100;
  a->path_cb = lv_anim_path_ease_out;
}

void lv_anim_run(lv_anim_t* a)
{
	int temp;
//	dbmsg("act_time:%d", a->act_time);
  if(a->act_time > 0)
  {
    if(a->act_time > a->time)
      a->act_time = a->time;

    int32_t new_value;
    new_value = a->path_cb(a);

    if(new_value != a->current_value)
    {
      a->current_value = new_value;
      /*Apply the calculated value*/
      if(a->exec_cb)
        a->exec_cb(a, new_value);
    }

    /*If the time is elapsed the animation is ready*/
    if(a->act_time >= a->time)
    {
			if(a->ready_cb)
				a->ready_cb(a);
    }
  }
	if(a->act_time <= a->time)
	{
		temp = lv_tick_elaps(last_timer_run);
		a->act_time += temp;
//		dbmsg("temp:%d",temp);
	}
  last_timer_run = HAL_GetTick();
}

lv_anim_t lv_anim_start(lv_anim_t* last, uint32_t end, int32_t start, lv_anim_exec_xcb_t exec_cb, uint32_t duration)
{
  lv_anim_t a = {0};
  lv_anim_init(&a);
  a.exec_cb = exec_cb;
  a.time = duration;

  a.start_value = start;
  a.current_value = start;
  a.end_value = end;
  return a;
}
