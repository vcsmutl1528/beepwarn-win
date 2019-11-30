
#include <stdio.h>
#include <time.h>
#include "abeep.h"

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
	b = AsyncMakeBeep (200); printf ("200 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (50); printf ("50 %d\n", b);
	Sleep (500);
	b = AsyncMakeBeep (40); printf ("40 %d\n", b);
	Sleep (500);
	AsyncMakeBeep (35); puts ("35");
	Sleep (500);
	AsyncMakeBeep (37); puts ("37");
	Sleep (500);
	AsyncMakeBeep (30); puts ("30");
	Sleep (500);
	AsyncMakeBeep (20); puts ("20");
	Sleep (500);
	AsyncMakeBeep (10); puts ("10");
	Sleep (500);

	if (!CloseBeepDevice ())
		puts ("CloseBeepDevice () error.");
	else	puts ("CloseBeepDevice () done.");

	return 0;
}
