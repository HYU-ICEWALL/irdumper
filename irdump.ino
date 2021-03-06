#include "IRremote.h"

IRsend irsend;

int recvPin = 11;
IRrecv irrecv(recvPin);

void setup()
{
	Serial.begin(9600);
	irrecv.enableIRIn();
}

void ircode(decode_results *results)
{
	// Panasonic has an Address
	if (results->decode_type == PANASONIC)
	{
		Serial.print(results->address, HEX);
		Serial.print(":");
	}

	// Print Code
	Serial.print(results->value, HEX);
}

//+=============================================================================
// Display encoding type
//
void encoding(decode_results *results)
{
	switch (results->decode_type)
	{
	default:
	case UNKNOWN:
		Serial.print("UNKNOWN");
		break;
	case NEC:
		Serial.print("NEC");
		break;
	case SONY:
		Serial.print("SONY");
		break;
	case RC5:
		Serial.print("RC5");
		break;
	case RC6:
		Serial.print("RC6");
		break;
	case DISH:
		Serial.print("DISH");
		break;
	case SHARP:
		Serial.print("SHARP");
		break;
	case JVC:
		Serial.print("JVC");
		break;
	case SANYO:
		Serial.print("SANYO");
		break;
	case MITSUBISHI:
		Serial.print("MITSUBISHI");
		break;
	case SAMSUNG:
		Serial.print("SAMSUNG");
		break;
	case LG:
		Serial.print("LG");
		break;
	case WHYNTER:
		Serial.print("WHYNTER");
		break;
	case AIWA_RC_T501:
		Serial.print("AIWA_RC_T501");
		break;
	case PANASONIC:
		Serial.print("PANASONIC");
		break;
	case DENON:
		Serial.print("Denon");
		break;
	}
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpInfo(decode_results *results)
{
	// Check if the buffer overflowed
	if (results->overflow)
	{
		Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
		return;
	}

	// Show Encoding standard
	Serial.print("Encoding  : ");
	encoding(results);
	Serial.println("");

	// Show Code & length
	Serial.print("Code      : ");
	ircode(results);
	Serial.print(" (");
	Serial.print(results->bits, DEC);
	Serial.println(" bits)");
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpRaw(decode_results *results)
{
	// Print Raw data
	Serial.print("Timing[");
	Serial.print(results->rawlen - 1, DEC);
	Serial.println("]: ");

	for (int i = 1; i < results->rawlen; i++)
	{
		unsigned long x = results->rawbuf[i] * USECPERTICK;
		if (!(i & 1))
		{ // even
			Serial.print("-");
			if (x < 1000)
				Serial.print(" ");
			if (x < 100)
				Serial.print(" ");
			Serial.print(x, DEC);
		}
		else
		{ // odd
			Serial.print("     ");
			Serial.print("+");
			if (x < 1000)
				Serial.print(" ");
			if (x < 100)
				Serial.print(" ");
			Serial.print(x, DEC);
			if (i < results->rawlen - 1)
				Serial.print(", "); //',' not needed for last one
		}
		if (!(i % 8))
			Serial.println("");
	}
	Serial.println(""); // Newline
}

unsigned int *dumpCode(decode_results *results)
{
	unsigned int *out = (unsigned int *)malloc(results->rawlen - 1);

	// Dump data
	for (int i = 1; i < results->rawlen; i++)
	{
		out[i - 1] = results->rawbuf[i] * USECPERTICK;
		Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
		if (i < results->rawlen - 1)
			Serial.print(","); // ',' not needed on last one
		if (!(i & 1))
			Serial.print(" ");
	}
	Serial.println("");

	return out;
}

unsigned int *irSignal = NULL;

void loop()
{
	const int khz = 38;
	decode_results results;

	if (irSignal != NULL)
	{
		irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz);
		Serial.println("Data replicated!");
	}
	else
	{
		Serial.println("data empty");
	}

	if (irrecv.decode(&results))
	{								   // Grab an IR code
		dumpInfo(&results);			   // Output the results
		dumpRaw(&results);			   // Output the results in RAW format
		irSignal = dumpCode(&results); // Output the results as source code
		// Serial.println("");    // Blank line between entries
		Serial.println("Data dumped!");
		irrecv.resume(); // Prepare for the next value
	}

	delay(2000);
}
