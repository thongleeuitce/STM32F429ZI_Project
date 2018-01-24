#include "main.h"
#include "queue.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#define STR_MAX_LENGTH	100

void uart_interrupt_init(void);
void timer3_interrupt_init(void);
void timer4_interrupt_init(void);

void uart_send(char *q);
void uart_receive(void);
void from_receive_to_send(queue_t * send, queue_t * receive);
void queue_push_string(queue_t * q, const char * string, const uint8_t length);
int get_data(queue_t q);
void option2_input_operand(int *a, int *b);
void option2_print_result(char *result);

void student_info(void);
void basic_operation(void);
void simple_led(void);
void advance_led(void);
void timer_counter(void);

void plus(void);
void subtract(void);
void multiply(void);
void divide(void);
void module(void);
void blink(void);
void countdown(void);
void stopwatch(void);

extern volatile int time_countdown;
extern volatile int time_countup;
extern volatile int flag_esc;
extern volatile int flag_multi_input;
extern volatile int flag_time_update;
extern volatile uint8_t b_receive_done;

extern queue_t queue_sender;
extern queue_t queue_receiver;

char* MAIN_MENU = "\n------------ MENU ---------------\n\
								Choose your option (1, 2, ..): \n\
								1. Student info\n\
								2. Basic operation\n\
								3. Simple led\n\
								4. Advance led\n\
								5. Timer counter\n\
								Your option: ";

char* OPTION1 = "\n1. Student info\n\
								ID: 14520899\n\
								Full name: Le Van Thong\n\
								ESC: return previous menu\n";

char* OPTION2 = "2. Basic operation( a,b,..):\n\
								a. Plus\n\
								b. Subtract\n\
								c. Multiply\n\
								d. Divide\n\
								e. Module\n\
								ESC: return previous menu\n\
								Your option: ";

char* OPTION3 = "3. Simple led\n\
								a. On Green\n\
								b. Off Green\n\
								c. Blink led\n\
								ESC: return previous menu\n\
								Your option: ";

char* OPTION4 = "4. Advance led(a, b, c)\n\
								a. Set set led\n\
								b. Set direction\n\
								c. Start\n\
								ESC: return previous menu\n\
								Your option: ";

char* OPTION5 = "5. Timer counter\n\
								a. Countdown\n\
								b. Stopwatch\n\
								ESC: return previous menu\n\
								Your option: ";

char* NEWLINE = "\n";

int choose;

queue_t queue_get_data;

char* c_result;
char a_result[100];	
int i_result;

int main()
{
	queue_init(&queue_sender);
	queue_init(&queue_receiver);
	queue_init(&queue_get_data);
	
	// Uart interrupt init
	uart_interrupt_init();

	flag_multi_input = 0;
	for(;;)
	{
		// Send option & wait for user input
		uart_send(MAIN_MENU);
		uart_receive();
		queue_get_data = queue_receiver;
		from_receive_to_send(&queue_sender, &queue_receiver);
		uart_send(NEWLINE);
		
		// Check input option
		// TODO: check if user input orther keys
		choose = get_data(queue_get_data);
		
		switch (choose)
		{
			case 1:
				student_info();
				break;
			case 2:
				basic_operation();
				break;
			case 3:
				simple_led();
				break;
			case 4:
				advance_led();
				break;
			case 5:
				timer_counter();
				break;
			default:
				break;
		}
	}
	return 0;
}

void student_info()
{
	uart_send(OPTION1);
	uart_send(NEWLINE);
	do
		uart_receive();
	while(flag_esc == 0);
	//reset queuce receiver and flag ESC
	queue_init(&queue_receiver);
	flag_esc = 0;
}

void basic_operation()
{
	for(;;)
	{
		// reset flag ESC
		flag_esc = 0;
		// Home screen option 2
		uart_send(NEWLINE);
		uart_send(OPTION2);
		uart_receive();
		if(flag_esc == 1)
		{
			flag_esc = 0;
			uart_send(NEWLINE);
			return;
		}		
		
		choose = queue_receiver.items[0];	

		// Print to terminal
		from_receive_to_send(&queue_sender, &queue_receiver);					
		uart_send(NEWLINE);
				
		// get data to check if wrong input
		if (choose < 97 || choose > 101)
			uart_send("Not a option!\n");
		//TODO: check if input_operand not a number
		switch (choose)
		{
			case 'a':
				plus();
				break;
			case 'b':
				subtract();
				break;
			case 'c':
				multiply();
				break;
			case 'd':
				divide();
				break;
			case 'e':
				module();
				break;
			default:
				break;
		}			
	}
}

void simple_led()
{
	for (;;)
	{
		uart_send(NEWLINE);
		uart_send(OPTION3);
		uart_receive();	
		
		if(flag_esc == 1)
		{
			flag_esc = 0;
			uart_send(NEWLINE);
			return;
		}		
		choose = queue_receiver.items[0];
		//printf to terminal
		from_receive_to_send(&queue_sender, &queue_receiver);				
		uart_send(NEWLINE);
		
		STM_EVAL_LEDInit(LED3);
		STM_EVAL_LEDInit(LED4);		
		
		while (choose < 97 || choose > 99)
		{
			uart_send("Not a option!\n");
			uart_send(NEWLINE);
			uart_send(OPTION3);
			uart_receive();
			
			queue_get_data = queue_receiver;
			// Print to terminal
			from_receive_to_send(&queue_sender, &queue_receiver);					
			uart_send(NEWLINE);
		
			choose = queue_get_data.items[0];
		}
		
		switch (choose)
		{
			case 'a':
				STM_EVAL_LEDOn(LED3);
				break;
			case 'b':
				STM_EVAL_LEDOff(LED3);
				break;
			case 'c':
				blink();
				break;
			default:
				break;
		}
	}
}

void blink()
{
	int i, j, times;
	char* INPUT_TIMES = "c. Blink led(Input number & press Enter)\nInput times you want to blink: ";
		
	/* Process for times input	*/
	queue_push_string(&queue_sender, NEWLINE, strlen(NEWLINE));
	uart_send(INPUT_TIMES);
	flag_multi_input = 1;
	uart_receive();
	flag_multi_input = 0;
	// Get data input
	queue_get_data = queue_receiver;
	
	// Print to terminal
	from_receive_to_send(&queue_sender, &queue_receiver);								
	uart_send(NEWLINE);
	
	// Convert to int
	// TODO: add timer
	times = get_data(queue_get_data);
	for (i = 0; i < times; i++)
	{
			STM_EVAL_LEDOn(LED3);
			STM_EVAL_LEDOn(LED4);
			for(j = 0; j < 3000000; j++);
			STM_EVAL_LEDOff(LED3);
			STM_EVAL_LEDOff(LED4);
			for(j = 0; j < 3000000; j++);
	}
}
void plus()
{
	// variables for calculation
	int operand1;
	int operand2;
	
	uart_send("\na. Plus (Input number & press Enter)");
	option2_input_operand(&operand1, &operand2);
	
	// Cal sum
	i_result = operand1 + operand2;
	
	// Convert back to string
	sprintf(a_result,"%d",i_result);
	c_result = a_result;
	
	// Print result
	option2_print_result(c_result);
}

void subtract()
{
	// variables for calculation
	int operand1;
	int operand2;

	uart_send("\nb. Subtract (Input number & press Enter)");
	option2_input_operand(&operand1, &operand2);
	
	// Cal sum
	i_result = operand1 - operand2;
	
	// Convert back to string
	sprintf(a_result,"%d",i_result);
	c_result = a_result;
	
	// Print result
	option2_print_result(c_result);
}

void multiply(void)
{
	// variables for calculation
	int operand1;
	int operand2;

	uart_send("\nc. Multiply (Input number & press Enter)");
	option2_input_operand(&operand1, &operand2);
	
	// Cal sum
	i_result = operand1 * operand2;
	
	// Convert back to string
	sprintf(a_result,"%d",i_result);
	c_result = a_result;
	
	// Print result
	option2_print_result(c_result);
}


void divide(void)
{
	// variables for calculation
	int operand1;
	int operand2;

	float div;

	uart_send("\nd. Divide (Input number & press Enter)");
	option2_input_operand(&operand1, &operand2);
	
	// Cal sum
	div = (float)(operand1) / (float)(operand2);
	
	// Convert back to string
	sprintf(a_result,"%0.2f",div);
	c_result = a_result;
	
	// Print result
	option2_print_result(c_result);	
}

void module(void)
{
	// variables for calculation
	int operand1;
	int operand2;

	float mol;

	uart_send("\ne. Module (Input number & press Enter)");
	option2_input_operand(&operand1, &operand2);
	
	// Cal sum
	mol = sqrt((float)(operand1 * operand1) + (float)(operand2 * operand2));
	
	// Convert back to string
	sprintf(a_result,"%0.2f",mol);
	c_result = a_result;
	
	// Print result
	option2_print_result(c_result);	
}

void advance_led()
{
	int check;
	for(;;)
	{
		flag_esc = 0;
		// Home screen option 4
		uart_send(NEWLINE);
		uart_send(OPTION4);
		uart_receive();
		
		if(flag_esc == 1)
		{
			flag_esc = 0;
			uart_send(NEWLINE);
			return;
		}
	}
}
void timer_counter()
{
	timer3_interrupt_init();
	for(;;)
	{
		// reset flag ESC
		flag_esc = 0;
		// Home screen option 5
		uart_send(NEWLINE);
		uart_send(OPTION5);
		uart_receive();
		if(flag_esc == 1)
		{
			flag_esc = 0;
			uart_send(NEWLINE);
			return;
		}		
		choose = queue_receiver.items[0];
		// Print to terminal
		from_receive_to_send(&queue_sender, &queue_receiver);					
		uart_send(NEWLINE);
		if (choose < 97 || choose > 101)
			uart_send("Not a option!\n");
		// get data to check if wrong input
		if (choose < 97 || choose > 101)
			uart_send("Not a option!\n");
		//TODO: check if input_operand not a number
		switch (choose)
		{
			case 'a':
				countdown();
				break;
			case 'b':
				stopwatch();
				break;
			default:
				break;
		}
	}
}
void countdown()
{
	int check;
	uart_send("\na. Countdown (Input number & press Enter)");
	uart_send("\nInput time countdown (s): ");
	flag_multi_input = 1;
	uart_receive();
	flag_multi_input = 0;

	// Get data input
	queue_get_data = queue_receiver;
	// Print to terminal
	from_receive_to_send(&queue_sender, &queue_receiver);	
	check = queue_get_data.items[0];
	while (check < 48 || check > 57)
	{
		/* Process for operand 1	*/
		uart_send("Not a number! Pleases input again!\n");
		uart_send("Input time countdown (s): ");
		flag_multi_input = 1;
		uart_receive();
		flag_multi_input = 0;

		// Get data input
		queue_get_data = queue_receiver;
		check = queue_get_data.items[0];
		
		// Print to terminal
		from_receive_to_send(&queue_sender, &queue_receiver);						
	}
	time_countdown = get_data(queue_get_data);	

	// enable Timer3 and interrupt for timer4
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	
	while(time_countdown != 0)
	{
		if(flag_time_update == 1)
		{
			sprintf(a_result,"%d",time_countdown);
			uart_send(a_result);
			uart_send("... ");
			flag_time_update = 0;
		}
	}
	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	
	uart_send("\nTime out !");
	uart_send(NEWLINE);
	uart_send("ESC: return previous menu");
	uart_send(NEWLINE);
	do
		uart_receive();
	while(flag_esc == 0);
	//reset queuce receiver and flag ESC
	queue_init(&queue_receiver);
	flag_esc = 0;
	
}
void stopwatch()
{
	timer4_interrupt_init();
	STM_EVAL_LEDInit(LED3);
	time_countup = 0;
	uart_send("\nb. Stopwatch");
	uart_send("\nPress 1 to Start\n");
	uart_send("ESC: return previous menu\n");
	uart_receive();
	if(flag_esc == 1)
		{
			flag_esc = 0;
			uart_send(NEWLINE);
			return;
		}		
	choose = queue_receiver.items[0];
	from_receive_to_send(&queue_sender, &queue_receiver);					
	uart_send(NEWLINE);
	while(choose != '1')
	{
		uart_send("Please press 1 to Start\n");
		uart_send("ESC: return previous menu\n");
		uart_receive();
		if(flag_esc == 1)
			{
				flag_esc = 0;
				uart_send(NEWLINE);
				return;
			}		
		choose = queue_receiver.items[0];
		from_receive_to_send(&queue_sender, &queue_receiver);			
		uart_send(NEWLINE);
	}
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	uart_send("\nCounting...\nPress 1 to Stop\n");
	uart_send("ESC: return previous menu\n");
	uart_receive();
	if(flag_esc == 1)
		{
			flag_esc = 0;
			uart_send(NEWLINE);
			TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
			TIM_Cmd(TIM4, DISABLE);
			time_countup = 0;
			return;
		}
	choose = queue_receiver.items[0];
	from_receive_to_send(&queue_sender, &queue_receiver);			
	uart_send(NEWLINE);
	while(choose != '1')
	{
		uart_send("Counting...\n");
		uart_send("Please press 1 to Stop\n");
		uart_send("ESC: return previous menu\n");
		uart_receive();
		if(flag_esc == 1)
			{
				flag_esc = 0;
				uart_send(NEWLINE);
				TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
				TIM_Cmd(TIM4, DISABLE);
				time_countup = 0;
				return;
			}		
		choose = queue_receiver.items[0];
		from_receive_to_send(&queue_sender, &queue_receiver);					
		uart_send(NEWLINE);
	}		
	
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
	TIM_Cmd(TIM4, DISABLE);
	STM_EVAL_LEDOff(LED3);

	uart_send("Time: ");
	sprintf(a_result,"%d", time_countup);
	uart_send(a_result);
	uart_send("s");
	uart_send(NEWLINE);
	uart_send("ESC: return previous menu\n");
	
	do
		uart_receive();
	while(flag_esc == 0);
	//reset queuce receiver and flag ESC
	queue_init(&queue_receiver);
	flag_esc = 0;
	time_countup = 0;
}
void option2_print_result(char *result)
{
	char* txtResult = "Result: ";	
	char* ESC = "ESC: return previous menu\n";
	
	uart_send(txtResult);
	uart_send(result);
	uart_send(NEWLINE);
	uart_send(ESC);
	
	flag_multi_input = 0;
	do
		uart_receive();
	while(flag_esc == 0);
	//reset queuce receiver and flag ESC
	queue_init(&queue_receiver);
	flag_esc = 0;
}

void option2_input_operand(int *a, int *b)
{	
	int operand1, operand2, check1, check2;
	char* NUM1_REQUEST = "Operand 1: ";
	char* NUM2_REQUEST = "Operand 2: ";
	check1 = 0;
	
	/* Process for operand 1	*/
	uart_send(NEWLINE);
	uart_send(NUM1_REQUEST);
	
	flag_multi_input = 1;
	uart_receive();
	
	// Get data input
	queue_get_data = queue_receiver;
	check1 = queue_get_data.items[0];
		
	// Print to terminal
	from_receive_to_send(&queue_sender, &queue_receiver);						
//	uart_send(NEWLINE);
	
	while (check1 < 48 || check1 > 57)
	{
		/* Process for operand 1	*/
		uart_send("Not a number! Pleases input again!\n");
		uart_send(NUM1_REQUEST);
		uart_receive();
	
		// Get data input
		queue_get_data = queue_receiver;
		check1 = queue_get_data.items[0];
		
		// Print to terminal
		from_receive_to_send(&queue_sender, &queue_receiver);						
	}
		
	// Convert to int
	operand1 = get_data(queue_get_data);
	*a = operand1;
	
	check2 = 0;
	// Process for operand 2
	uart_send(NUM2_REQUEST);
	uart_receive();
	
	// Get data input	
	queue_get_data = queue_receiver;
	check2 = queue_get_data.items[0];
	
	// Print to terminal	
	from_receive_to_send(&queue_sender, &queue_receiver);
//	uart_send(NEWLINE);	
	
	while (check2 < 48 || check2 > 57)
	{
		uart_send("Not a number! Pls input again!\n");
		// Process for operand 2
		uart_send(NUM2_REQUEST);
		uart_receive();
	
		// Get data input	
		queue_get_data = queue_receiver;
		check2 = queue_get_data.items[0];
	
		// Print to terminal	
		from_receive_to_send(&queue_sender, &queue_receiver);
		uart_send(NEWLINE);		
	}
	
	// Convert to int
	operand2 = get_data(queue_get_data);
	*b = operand2;
}

void uart_interrupt_init()
{
	GPIO_InitTypeDef gpio_init;
	USART_InitTypeDef usart_init;
	NVIC_InitTypeDef nvic_init;

	GPIO_StructInit(&gpio_init);
	USART_StructInit(&usart_init);
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &gpio_init);

	usart_init.USART_BaudRate = 9600;
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart_init.USART_Parity = USART_Parity_No;
	usart_init.USART_StopBits = USART_StopBits_1;
	usart_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &usart_init);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	nvic_init.NVIC_IRQChannel = USART3_IRQn;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&nvic_init);

	USART_Cmd(USART3, ENABLE);
}
void timer3_interrupt_init()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef nvic_timer;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/4)/1000000)-1;   // frequency = 1MHz
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;														// 1ms
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	nvic_timer.NVIC_IRQChannel = TIM3_IRQn;
  nvic_timer.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_timer.NVIC_IRQChannelSubPriority = 1;
  nvic_timer.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_timer);   
	
	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
	TIM_Cmd(TIM3, DISABLE);
}
void timer4_interrupt_init()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef nvic_timer;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/4)/1000000)-1;   // frequency = 1MHz
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;														// 1ms
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	nvic_timer.NVIC_IRQChannel = TIM4_IRQn;
  nvic_timer.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_timer.NVIC_IRQChannelSubPriority = 1;
  nvic_timer.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_timer);   
	
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
	TIM_Cmd(TIM4, DISABLE);
}

void queue_push_string(queue_t * q, const char * string, const uint8_t length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		queue_push(q, string[i]);
	}
}

void uart_send(char *q)
{
	queue_push_string(&queue_sender, q, strlen(q));
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

void uart_receive()
{
	b_receive_done = 0;
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	while(b_receive_done == 0);
}

void from_receive_to_send(queue_t * send, queue_t * receive)
{
	uint8_t item;
	uint8_t b_success = 0;
	while(queue_is_empty(receive) == 0)
	{
		item = queue_pop(receive, &b_success);
		queue_push(send, item);
	}
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

int get_data(queue_t q)
{
	char temp[100];
	int i = 0;
	uint8_t b_success = 0;
	
	while (queue_is_empty(&q) == 0)
	{
		temp[i] = (char) queue_pop(&q, &b_success);
//		temp[i] = (char)(q.items[i]);
//		q.capacity--;
//		q.items[i] = q.items[i+1];
//		i++;
		i++;
	}
	
	return atoi(temp);
}
