Viet Nguyen
CMPE 13/L

COLLEAGUES WORKED WITH: Kevin Jesubalin and I discussed how to append the string
                        in lab main
SOURCES USED: Lab manual pseudocode and state diagram
HEAP SIZE: 4000 at least

INTRODUCTION:

Lab6 was given to students to test how well they could implement event driven
programming to control the uno32 board. The lab in specific asked students to
implement a Morse Decoder. Users could input their messages with BTN4 and 
their message would be decoded and represented on line 2 below it. 


DESCRIPTION:

I approached this lab by reading the lab manual, and a lot of unit testing. In
order to complete a lab of this magnitude and with as many new concepts it 
introduced, I had to take each part one bit at a time. To start I had to build
the binary tree library, starting with TreeCreate. To create a balanced tree,
I had to recursively travel through each level and add left and right nodes 
according to the string given. I also accomplished the PrintTree function by 
following the lab manual pseudocode and printed the spaces recursively first
on the right (because the right child is on top when flipped) and then on the 
left. Next was to implement the Morse library. I followed the state diagram for
MorseCheckEvents and basically figured out MorseDecode by following the 
description for it in the header. The main lab8 file took a bit because I had 
to figure out how to display/append the strings to the OLED, but after 
discussing it with KJ I was able to use stringcpy and stringcat to append them
and then send them to OledDrawString(). After settling the display, the rest 
was taken care of by handling each case of MorseEvents.

CONCLUSION:

This lab was very fair and enough time was given towards it. I wished that
there wasn't a holiday on Monday so I could have went to lab to work on it more
but ultimately it was fair and fun. Debugging using the buttons was the hardest
part for MorseCheckEvents(). If I was able to use the debugger and step into 
it during the time of my button press it would have been a bit easier. But
ultimately it was accomplished and matched to the best of my ability