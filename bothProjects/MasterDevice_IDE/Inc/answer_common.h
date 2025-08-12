#ifndef ANSWER_COMMON_H_
#define ANSWER_COMMON_H_

struct cmd_result {		// rename

	uint32_t value_1;
};

struct answer_ {

	const uint8_t * data;
	int16_t length;
	void (*Handler)(struct __VarialbeAnswer * Answer, const struct cmd_result * params);
};


#endif
