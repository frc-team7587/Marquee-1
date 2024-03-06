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
    private static final String TOO_LONG =
        "It seems to me that it was far from right for the Professor of English Literature "
        + "in Yale, the Professor of English Literature in Columbia, and Wilkie Collins "
        + "to deliver opinions on Cooper's literature without having read some of it. "
        + "It would have been much more decorous to keep silent and let persons talk "
        + "who have read Cooper.\n\n"

        + "Cooper's art has some defects. In one place in 'Deerslayer,' "
        + "and in the restricted space of two-thirds of a page, Cooper "
        + "has scored 114 offences against literary art out of a "
        + "possible 115. It breaks the record.\n\n"

        + "There are nineteen rules governing literary art in the "
        + "domain of romantic fiction—some say twenty-two. In "
        + "Deerslayer Cooper violated eighteen of them. ";

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
            // Test error handling.
            case 9:
              announceColorErrorHandlingTest();
              break;
            case 10:
              overSizedColorTest();
              break;
            case 11:
              negativeColorTest();
              break;
            case 12:
              announceDelay1Test();
              break;
            case 13:
              negativeDelay1Test();
              break;
            case 14:
              announceDelay2Test();
              break;
            case 15:
              negativeDelay2Test();
              break;
            case 16:
              announceOversizeMessageTest();
              break;
            case 17:
              oversizeMessageTest();
              break;
          }
    
          cycleCount = (++cycleCount) % 18;
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

  private void announceColorErrorHandlingTest() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("RGB Error Tests Follows")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }

  private void overSizedColorTest() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Over Size Color")
        .setForegroundRed(999)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }

  private void negativeColorTest() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Over Size Color")
        .setForegroundRed(-999)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }

  private void announceDelay1Test() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Delay 1 Test Follows")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }

  private void negativeDelay1Test() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Negative delay 1 Test")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(-1);
    displayConnection.send(smoothCrawl);
  }

  private void announceDelay2Test() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Delay 2 Test Follows")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }

  private void negativeDelay2Test() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Negative delay 2 Test")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40)
        .setDelay2(-1);
    displayConnection.send(smoothCrawl);
  }

  private void announceOversizeMessageTest() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("Oversize Message Test Follows")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }

  private void oversizeMessageTest() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText(TOO_LONG)
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    displayConnection.send(smoothCrawl);
  }
}
