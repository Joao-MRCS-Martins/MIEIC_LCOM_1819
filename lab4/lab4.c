// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include <i8042.h> 
#include <mouse.h>
#include <kbc.h>

extern int hook_id;
extern uint8_t data;
extern int ih_bool;
int size=0;
extern int _int_handler;

static bool cx;
static bool cy;

//gesture buttons checker
static bool lb_pressed = false;
static bool rb_pressed = false;
// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) 
{
  int ipc_status=0;
  message msg;
  uint8_t r;
  uint8_t irq_set;  


  if(trymousecmd(STREAM_MODE) == 1)
    return 1;

  if(trymousecmd(EN_DATA_REP) == 1)
    return 1;

  if(mouse_subscribe_int(&irq_set) != OK)
    return 1;

  while(cnt != 0)
    {
      // Get a request message.
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
      {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) //received notification
      {
        switch (_ENDPOINT_P(msg.m_source))
        {
          //hardware interrupt notification
          case HARDWARE : 
          {
            //handles keyboard interruptions
            if (msg.m_notify.interrupts & BIT(irq_set))
            {
              struct packet pp;

              //mouse interrupt handler
              mouse_ih();

              switch(size)
              {
                case 0:
                {
                  if(FLAG_BYTE1 & data)
                  {
                   pp.bytes[size]=data;
                   pp.rb= (data & RB);
                   pp.lb=(data & LB);
                   pp.mb=(data & MB);
                   pp.x_ov=(data & X_OVF);
                   pp.y_ov=(data & Y_OVF);

                   //cx and cy save byte SIGN to verify if bytes 1 and 2 are complement or not
                   cx = (data & X_SIGN);
                   cy = (data & Y_SIGN);
                   size++;
                  }
                  else
                  {
                  printf("Error: Not first byte");
                  size = 0;
                  }
                break;
                }
                case 1:
                {
                   pp.bytes[size] = data;
                  if(cx)
                  {
                     pp.delta_x = data - 256;
                  }
                  else
                  {
                  pp.delta_x= data;
                  }
                  size++;
                  break;
                }
                case 2:
                {
                  pp.bytes[size] = data;
                  if(cy)
                  {
                     pp.delta_y = data - 256;
                  }
                  else
                  {
                  pp.delta_y = data;
                  }
                  
                  mouse_print_packet(&pp);
                  size = 0;
                  cnt--;
                  break;
                }
                
              }
            }
             break;
          }
          default :
            break; // no other notifications expected: do nothing
        }
      }
    }
  
  if(mouse_unsubscribe_int() != OK)
  return 1;

  if(trymousecmd(DIS_DATA_REP) == 1)
  return 1;
  
  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {

  struct packet pp;
  int c = 0;
  size = 0;


  while (cnt > 0) {
    //reads information of packets 

    //read data command needs to be send for each packet
    if (trymousecmd(READ_DATA) == 1)
      return 1;


    while (c < 3) {
      //reads information of packet
  
      if (read_information_pl(&data, MOUSE_IRQ) != OK)
        return 1;
        
      //fills struct packet
      switch (size) {
        case 0: {
          if (FLAG_BYTE1 & data) {
            pp.bytes[size] = data;
            pp.rb = (data & RB);
            pp.lb = (data & LB);
            pp.mb = (data & MB);
            pp.x_ov = (data & X_OVF);
            pp.y_ov = (data & Y_OVF);
            cx = (data & X_SIGN);
            cy = (data & Y_SIGN);
            size++;
            c++;
          }
          else {
            printf("Error: Not first byte \n");
            size = 0;
          }
          break;
        }
        case 1: {
          pp.bytes[size] = data;
          if (cx) {
            pp.delta_x = data - 256;
          }
          else {
            pp.delta_x = data;
          }
          size++;
          c++;
          break;
        }
        case 2: {
          pp.bytes[size] = data;
          if (cy) {
            pp.delta_y = data - 256;
          }
          else {
            pp.delta_y = data;
          }

          c++;
          mouse_print_packet(&pp);
          size = 0;
          cnt--;
          break;
        }
      }  
      
    }

    //waits between packets
    tickdelay(micros_to_ticks(period*1000));
    c = 0;
  }

  
  uint8_t byte_cmd;

  byte_cmd = minix_get_dflt_kbc_cmd_byte();

  if (issue_cmd_b(byte_cmd) == 1)
    return 1;
 
  if (trymousecmd(STREAM_MODE) == 1)
    return 1;

  if (trymousecmd(DIS_DATA_REP) == 1)
    return 1;


  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  
  int ipc_status=0;
  message msg;
  uint8_t r;
  uint8_t irq_set_tim;
  uint8_t irq_set_mouse;
  int size=0;

  if(trymousecmd(EN_DATA_REP) == 1)
    return 1;

  //subscribe mouse
  if(mouse_subscribe_int(&irq_set_mouse) != OK)
    return 1;

  //subscribe timer
  if(timer_subscribe_int(&irq_set_tim) != OK)
    return 1;

  while(_int_handler < idle_time*60)
    {
      // Get a request message.
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
      {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) //received notification
      {
        switch (_ENDPOINT_P(msg.m_source))
        {
          // hardware interrupt notification
          case HARDWARE : 

            //handles keyboard interruptions
            if (msg.m_notify.interrupts & BIT(irq_set_mouse))
            {
              _int_handler=0;
              struct packet pp;

              //mouse interrupt handler
              mouse_ih();
              
              if(ih_bool!= 0)
                break;
         
              //fill struct
              switch(size)
              {
                case 0:
                
                  if(FLAG_BYTE1 & data)
                  {
                   pp.bytes[size]=data;
                   pp.rb= (data & RB);
                   pp.lb=(data & LB);
                   pp.mb=(data & MB);
                   pp.x_ov=(data & X_OVF);
                   pp.y_ov=(data & Y_OVF);
                   cx = (data & X_SIGN);
                   cy = (data & Y_SIGN);
                   size++;
                  }
                  else
                  {
                  printf("Error: Not first byte");
                  size--;
                  }
                break;
                
                case 1:
                
                   pp.bytes[size] = data;
                  if(cx)
                  {
                    pp.delta_x = data - 256;
                  }
                  else
                  {
                  pp.delta_x= data;
                  }
                  size++;
                  break;
                
                case 2:
                
                  pp.bytes[size] = data;

                  if(cy)
                  {
                     pp.delta_y = data - 256;
                  }
                  else
                  {
                  pp.delta_y = data;
                  }

                  mouse_print_packet(&pp);
                  size = 0;
                  break;
                default:
                  break;                
              }
            }

            //handles timer interruptions
            else if(msg.m_notify.interrupts & BIT(irq_set_tim))
            {                                        
              timer_int_handler();
            }

          break;
           
        default:
          break;
        }
      }
      else
      {
        printf("Failed to receive notification from ipc. \n");
        return 1;
      }
    }

    
  if(timer_unsubscribe_int() != OK)
    return 1;

  if(mouse_unsubscribe_int() != OK)
    return 1;
  
  if(trymousecmd(DIS_DATA_REP) == 1)
    return 1;

 return 0;
}

/////////////////////////////////////////////////////////////////////
//////////////////////// TEST GESTURE ///////////////////////////////
/////////////////////////////////////////////////////////////////////

//function to detect mouse related events
struct mouse_ev * (mouse_det_evt) (struct packet * pp)
{
  enum mouse_ev_t et = BUTTON_EV; //for initialization purposes only, not meaningful
  struct mouse_ev* evt = malloc (sizeof *evt);
  
  if(pp->delta_x != 0 || pp->delta_y !=0)
  {
    et = MOUSE_MOV;
    (*evt).type=et;
    (*evt).delta_x=pp->delta_x;
    (*evt).delta_y=pp->delta_y;
  }
  else if (pp->lb && !pp->rb && !pp->mb)
  {
    et = LB_PRESSED;
    lb_pressed = true;
    (*evt).type=et;
  }
  else if(pp->rb && !pp->lb && !pp->mb)
  {
    et=RB_PRESSED;
    rb_pressed = true;
    (*evt).type=et;
  }
  else if (!pp->rb && !pp->lb && !pp->mb && lb_pressed)
  {
    et = LB_RELEASED;
    lb_pressed = false;
    (*evt).type=et;
  }
  else if (!pp->rb && !pp->lb && !pp->mb && rb_pressed)
  {
    et = RB_RELEASED;
    rb_pressed = false;
    (*evt).type=et;
  }
  else
  {
    et = BUTTON_EV;
    (*evt).type=et;
  }

  return evt;
}

//checks whether the state machine is in a specific state and evaluate the movement
//of the drawn line to check if its valid
int (check_drawline)(struct mouse_ev evt,int state, uint8_t tolerance)
{
  if(state == 1)
  {
    //when drawing left line check if it goes in right direction and when have unusual movements
    //it respects the tolerance
    if((evt.delta_x <0 && abs(evt.delta_x) > tolerance) || (evt.delta_y<0 && abs(evt.delta_y) > tolerance))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else if(state == 2)
  {
    //between lines the mouse can't move more than the tolerated
    if(abs(evt.delta_x) > tolerance || abs(evt.delta_y) > tolerance )
      return 1;
    else
      return 0;
  }
  else if(state == 3)
  {
    //when drawing right line checks if it goes in right direction and when it respects the tolerance
    if((evt.delta_x <0 && abs(evt.delta_x) > tolerance) || (evt.delta_y>0 && evt.delta_y > tolerance))
    {
      return 1;
    } 
    else
    {
      return 0;
    }
  }
  else
  {
    printf("Error: invalid state for evaluation. %d \n", state);
    return 1;
  }
}

//this function parses the packet through mouse interruptions
int mouse_packet_parser(struct packet *pp, uint8_t irq_set_mouse)
{
  int ipc_status=0;
  message msg;
  uint8_t r;
  int size=0;
    
  
  
    while(true)
    {
      // Get a request message.
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
          printf("driver_receive failed with: %d", r);
          continue;
        }
      
      if (is_ipc_notify(ipc_status)) //received notification
      {
        switch (_ENDPOINT_P(msg.m_source))
        {
          // hardware interrupt notification
          case HARDWARE : 
            
            //handles keyboard interruptions
            if (msg.m_notify.interrupts & BIT(irq_set_mouse))
            {  
              //mouse interrupt handler            
              mouse_ih();

              switch(size)
              {
                case 0:
                
                  if(FLAG_BYTE1 & data)
                  {
                    (*pp).bytes[size]=data;
                    (*pp).rb= (data & RB);
                    (*pp).lb=(data & LB);
                    (*pp).mb=(data & MB);
                    (*pp).x_ov=(data & X_OVF);
                    (*pp).y_ov=(data & Y_OVF);
                    
                    //cx and cy save byte SIGN to verify if bytes 1 and 2 are complement or not
                    cx = (data & X_SIGN);
                    cy = (data & Y_SIGN);
                    size++;
                  }
                  else
                  {
                    printf("Error: Not first byte");
                    size=0;
                  }
      
                  break;
                  
                case 1:
                
                  (*pp).bytes[size] = data;
                  if(cx)
                  {
                    (*pp).delta_x = data - 256;
                  }
                  else
                  {
                    (*pp).delta_x= data;
                  }
                
                  size++;
                  break;
                  
                case 2:

                  (*pp).bytes[size] = data;
                  if(cy)
                  {
                    (*pp).delta_y = data - 256;
                  }
                  else
                  {
                    (*pp).delta_y = data;
                  }

                  //the packet has been parsed, return with success
                  return 0;

                  break;

              }
            }
            break;
          default:
            break;
        }
      }
      else
        {
          printf("Failed to receive notification from ipc. \n");
          return 1;
        }
    }
}

//function that implements a state machine, detecting a given gesture, drawing an AND operator while pressing
//left and right buttons, in different times
int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  
  int state = 0; //initial state : 0 || drawing left line with left button pressed : 1 || right button pressed only : 2 || drawing right line with right button pressed : 3 || final state : 4
  struct packet pp;
  struct mouse_ev* m_evt;
  uint8_t irq_set_mouse;

  uint16_t x_total =0;
  uint16_t y_total =0;

  if(trymousecmd(STREAM_MODE) != OK)
    return 1;

  if(trymousecmd(EN_DATA_REP) != OK)
    return 1;

  if(mouse_subscribe_int(&irq_set_mouse) != OK)
    return 1;
  

  //loop handling the state machine. IMPLEMENT MACROS SAVING INIT AND FINAL STATES
  while(state != 4)
  {
    //fills packet with information
    mouse_packet_parser(&pp,irq_set_mouse);
    
    //prints the packet to the screen
    mouse_print_packet(&pp);

    //detects events and fills the struct
    m_evt = mouse_det_evt(&pp);

    //checks which state is in to determine next step
    switch(state)
    {
      case 0:
      
        if((*m_evt).type == LB_PRESSED)
        {
          state = 1;
        }
        break;
      case 1:
        
        if((*m_evt).type == MOUSE_MOV) 
        {
          x_total += (*m_evt).delta_x;
          y_total += (*m_evt).delta_y;
          if(check_drawline((*m_evt), state, tolerance) == 1) 
          {
            state = 0;
            x_total = 0;
            y_total = 0;
          }
          break;
        } 
        else if((*m_evt).type == LB_RELEASED) 
        {
          if (x_total >= x_len && y_total/x_total > 1) 
          {
            state = 2;
            x_total = 0;
            y_total = 0;
            break;
          } 
          else 
          {
            state = 0;
            x_total = 0;
            y_total = 0;
            break;
          }
        } 
        else 
        {
          state = 0;
          x_total = 0;
          y_total = 0;
          break;
        }

      case 2: 
        if((*m_evt).type == RB_PRESSED) 
        {
          state = 3;
          x_total = 0;
          y_total = 0;
        }
        else if((*m_evt).type == MOUSE_MOV) 
        {
          if(check_drawline((*m_evt),state,tolerance) == 1)
          {
            state=0;
            x_total = 0;
            y_total = 0;
          }
        } 
        else if ((*m_evt).type == LB_PRESSED)
        {
            state=1;
            x_total = 0;
            y_total = 0;
        }
        else
        {
          state = 0;
          x_total = 0;
          y_total = 0;
        }
        break;
      case 3:
        if((*m_evt).type == MOUSE_MOV) 
        {
          x_total += (*m_evt).delta_x;
          y_total -= (*m_evt).delta_y;
          if (check_drawline((*m_evt), state, tolerance) == 1) 
          {
            state=0;
            x_total = 0;
            y_total = 0;
          }
          break;
        }
        else if((*m_evt).type == RB_RELEASED) 
        {
          if(x_total >= x_len && abs(y_total/x_total) > 1) 
          {
            state = 4;
          } else {
            state = 0;
          }
          x_total = 0;
          y_total = 0;
          break;
        } 
        else 
        {
          state = 0;
          x_total = 0;
          y_total = 0;
          break;
        }
      default:
        printf("Error: invalid state. \n");
        break;
    }
  }

  //returning to default state by unsubscribing interruptions
  if(mouse_unsubscribe_int() != OK)
    return 1;

  //and disabling the data reporting
  if(trymousecmd(DIS_DATA_REP) != OK)
    return 1;
 

  return 0;
}

