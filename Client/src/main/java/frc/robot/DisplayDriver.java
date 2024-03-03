package frc.robot;

import org.metuchenmomentum.marquee.DisplayCommand;
import org.metuchenmomentum.marquee.DisplayConnection;
import org.metuchenmomentum.marquee.DisplayMessage;

/**
 * Test driver for the Metuchen Momentum marquee. The driver cycles
 * the display through test messages, displaying each message for
 * 6000 ms.
 */
public class DisplayDriver {

    private static final String MO_NO = "7587";

    private final DisplayConnection displayConnection;

    private int callCount;
    private int cycleCount;
  

    public DisplayDriver(DisplayConnection displayConnection) {
        this.displayConnection = displayConnection;
        callCount = 0;
        cycleCount = 0;
    }

    public void init() {
        rainbowPattern();
    }
    
    /**
     * The robot's {@code robotPeriodic} method should invoke this every cycle,
     * that is, every 20 milliseconds.
     */
    public void robotPeriodic() {
        callCount %= 300;
        if (callCount == 0) {
          switch (cycleCount) {
            case 0:
              blueTeamNumber();
              break;
            case 1:
              redTeamNumber();
              break;
            case 2:
            default:
              errorPattern();
              break;
            case 3:
              singlePixelNaturalOrder();
              break;
            case 4:
              singlePixelLeftToRight();
              break;
            case 5:
              fillWithGreen();
              break;
            case 6:
              scrollText();
              break;
            case 7:
              smoothTextCrawl();
              break;
            case 8:
              rainbowPattern();
              break;
          }
    
          cycleCount = (++cycleCount) % 9;
        }
        ++callCount;
      }
  private void blueTeamNumber() {
        // Fill the display with blue.
        DisplayMessage setBlue = new DisplayMessage()
        .setText(MO_NO)
        .setForegroundBlue(127)
        .setDisplayCommand(DisplayCommand.STATIC_TEXT);
      int bytesSent = displayConnection.send(setBlue);
      System.out.println(
        "Blue team number command >>>" + setBlue.toString() + "<<<");
      System.out.println(
        "Blue team number message sent to display, length = " + bytesSent + " bytes");
  }
  private void redTeamNumber() {
    // Fill the display with blue.
    DisplayMessage setBlue = new DisplayMessage()
      .setText(MO_NO)
      .setForegroundRed(127)
      .setDisplayCommand(DisplayCommand.STATIC_TEXT);
    int bytesSent = displayConnection.send(setBlue);
    System.out.println(
      "Red team number message sent to display, length = " + bytesSent + " bytes");
  }

  private void fillWithGreen() {
    DisplayMessage greenFill = new DisplayMessage()
        .setForegroundGreen(63)
        .setDisplayCommand(DisplayCommand.FILL_WITH_COLOR);
    int bytesSent = displayConnection.send(greenFill);
    System.out.println(
      "Green fill pattern command sent to display, length = " + bytesSent);
  }
  private void errorPattern() {
    DisplayMessage displayError = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.ERROR)
        .setDelay1(100);
    int bytesSent = displayConnection.send(displayError);
    System.out.println(
      "Display error pattern command set to display, length = " + bytesSent);
  }

  private void rainbowPattern() {
    DisplayMessage displayRainbow = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.RIPPLING_RAINBOW)
        .setDelay1(100);
    displayConnection.send(displayRainbow);
}

  private void scrollText() {
    DisplayMessage textCrawl = new DisplayMessage()
        .setText("Text crawl ...")
        .setDisplayCommand(DisplayCommand.SCROLLING_TEXT)
        .setForegroundGreen(32)
        .setForegroundBlue(32)
        .setDelay1(50)
        .setDelay2(50);
    int bytesSent = displayConnection.send(textCrawl);
    System.out.println(
      "Text crawl command sent to display, length = " + bytesSent);
  }

  private void singlePixelNaturalOrder() {
    DisplayMessage naturalOrder = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.SINGLE_PIXEL_NATURAL_ORDER)
        .setForegroundRed(32)
        .setForegroundGreen(32)
        .setDelay1(50);
    displayConnection.send(naturalOrder);
  }

  private void singlePixelLeftToRight() {
    DisplayMessage leftToRight = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.SINGLE_PIXEL_LEFT_TO_RIGHT)
        .setForegroundRed(32)
        .setForegroundBlue(32)
        .setDelay1(50);
    displayConnection.send(leftToRight);
  }

  private void smoothTextCrawl() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("---Smooth---")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }
}
