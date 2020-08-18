
#include <stdio.h>
#include <time.h>
#include "asynbeep.h"

int main ()
{
	clock_t t;
	BOOL b;

	if (!OpenBeepDevice ()) {
		puts ("OpenBeepDevice () failed.");
		return 1;
	} else	puts ("OpenBeepDevice () succedded.");

	t = clock ();
	AsyncBeep (800, 200);
	Sleep (1000);
	puts ("[Measured] Set");
	printf ("[%d] 1000\n", clock () - t);
	AsyncMakeBeep (1000);
	Sleep (300);
	printf ("[%d] 1300\n", clock () - t);
	AsyncMakeBeep (0);
	Sleep (700);
	printf ("[%d] 2000\n", clock () - t);
	AsyncBeep (800, 1000);
	Sleep (200);
	printf ("[%d] 2200\n", clock () - t);
	AsyncMakeBeep (0);
	Sleep (800);
	AsyncMakeBeep (100); puts ("100");
	Sleep (500);
	puts ("Freq/Res");
	b = AsyncMakeBeep (200); printf ("200 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (50); printf ("50 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (40); printf ("40 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (37); printf ("37 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (35); printf ("35 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (30); printf ("30 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (20); printf ("20 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (10); printf ("10 %d\n", b);
	Sleep (500);

	if (!CloseBeepDevice ())
		puts ("CloseBeepDevice () error.");
	else	puts ("CloseBeepDevice () done.");

	return 0;
}
