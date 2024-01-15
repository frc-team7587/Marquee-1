// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.SPI;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

import org.metuchenmomentum.marquee.DisplayCommand;
import org.metuchenmomentum.marquee.DisplayMessage;
import org.metuchenmomentum.marquee.SpiDisplayConnection;

/**
 * The VM is configured to automatically run this class, and to call the functions corresponding to
 * each mode, as described in the TimedRobot documentation. If you change the name of this class or
 * the package after creating this project, you must also update the build.gradle file in the
 * project.
 */
public class Robot extends TimedRobot {
  private static final String kDefaultAuto = "Default";
  private static final String kCustomAuto = "My Auto";
  private static final String moNo = "7587";
  private String m_autoSelected;
  private final SendableChooser<String> m_chooser = new SendableChooser<>();
  private SpiDisplayConnection spi0DisplayConnection;
  private SPI spi1;
  private DigitalOutput digOut0;
  private int callCount;
  private int cycleCount;

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  @Override
  public void robotInit() {
    m_chooser.setDefaultOption("Default Auto", kDefaultAuto); 
    m_chooser.addOption("My Auto", kCustomAuto);
    SmartDashboard.putData("Auto choices", m_chooser);
    spi0DisplayConnection = new SpiDisplayConnection(SPI.Port.kOnboardCS0);

    // Fill the display with blue.
    DisplayMessage setBlue = new DisplayMessage()
      .setForegroundBlue(127)
      .setDisplayCommand(DisplayCommand.FILL_WITH_COLOR);
    int bytesSent = spi0DisplayConnection.send(setBlue);
    System.out.println("Message sent to display, length = " + bytesSent + " bytes");
    spi1 = new SPI(SPI.Port.kOnboardCS1);
    spi1.setMode(SPI.Mode.kMode0);
    spi1.setClockRate(100000);
    System.out.println("Second SPI output configured.");
    // bytesSent = spi1.write(dummyMessage, dummyMessage.length);
    // System.out.println("" + bytesSent + " bytes written to SPI1");
    digOut0 = new DigitalOutput(0);
    digOut0.setPWMRate(1000);
    digOut0.enablePWM(0.5);
    System.out.println("Digital Output 0 configured for PWM at 100000 kHz.");
    callCount = 0;
    callCount = 0;
    cycleCount = 0;
    System.out.println("Robot initialized.");
  }

  /**
   * This function is called every 20 ms, no matter the mode. Use this for items like diagnostics
   * that you want ran during disabled, autonomous, teleoperated and test.
   *
   * <p>This runs after the mode specific periodic functions, but before LiveWindow and
   * SmartDashboard integrated updating.
   */
  @Override
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

  /**
   * This autonomous (along with the chooser code above) shows how to select between different
   * autonomous modes using the dashboard. The sendable chooser code works with the Java
   * SmartDashboard. If you prefer the LabVIEW Dashboard, remove all of the chooser code and
   * uncomment the getString line to get the auto name from the text box below the Gyro
   *
   * <p>You can add additional auto modes by adding additional comparisons to the switch structure
   * below with additional strings. If using the SendableChooser make sure to add them to the
   * chooser code above as well.
   */
  @Override
  public void autonomousInit() {
    m_autoSelected = m_chooser.getSelected();
    // m_autoSelected = SmartDashboard.getString("Auto Selector", kDefaultAuto);
    System.out.println("Auto selected: " + m_autoSelected);
  }

  /** This function is called periodically during autonomous. */
  @Override
  public void autonomousPeriodic() {
    switch (m_autoSelected) {
      case kCustomAuto:
        // Put custom auto code here
        break;
      case kDefaultAuto:
      default:
        // Put default auto code here
        break;
    }
  }

  /** This function is called once when teleop is enabled. */
  @Override
  public void teleopInit() {}

  /** This function is called periodically during operator control. */
  @Override
  public void teleopPeriodic() {}

  /** This function is called once when the robot is disabled. */
  @Override
  public void disabledInit() {}

  /** This function is called periodically when disabled. */
  @Override
  public void disabledPeriodic() {}

  /** This function is called once when test mode is enabled. */
  @Override
  public void testInit() {}

  /** This function is called periodically during test mode. */
  @Override
  public void testPeriodic() {}

  /** This function is called once when the robot is first started up. */
  @Override
  public void simulationInit() {}

  /** This function is called periodically whilst in simulation. */
  @Override
  public void simulationPeriodic() {}

  private void blueTeamNumber() {
        // Fill the display with blue.
        DisplayMessage setBlue = new DisplayMessage()
        .setText(moNo)
        .setForegroundBlue(127)
        .setDisplayCommand(DisplayCommand.STATIC_TEXT);
      int bytesSent = spi0DisplayConnection.send(setBlue);
      System.out.println(
        "Blue team number command >>>" + setBlue.toString() + "<<<");
      System.out.println(
        "Blue team number message sent to display, length = " + bytesSent + " bytes");
  }
  private void redTeamNumber() {
    // Fill the display with blue.
    DisplayMessage setBlue = new DisplayMessage()
      .setText(moNo)
      .setForegroundRed(127)
      .setDisplayCommand(DisplayCommand.STATIC_TEXT);
    int bytesSent = spi0DisplayConnection.send(setBlue);
    System.out.println(
      "Red team number message sent to display, length = " + bytesSent + " bytes");
  }

  private void fillWithGreen() {
    DisplayMessage greenFill = new DisplayMessage()
        .setForegroundGreen(63)
        .setDisplayCommand(DisplayCommand.FILL_WITH_COLOR);
    int bytesSent = spi0DisplayConnection.send(greenFill);
    System.out.println(
      "Green fill pattern command sent to display, length = " + bytesSent);
  }
  private void errorPattern() {
    DisplayMessage displayError = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.ERROR)
        .setDelay1(100);
    int bytesSent = spi0DisplayConnection.send(displayError);
    System.out.println(
      "Display error pattern command set to display, length = " + bytesSent);
  }

  private void rainbowPattern() {
    DisplayMessage displayRainbow = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.RIPPLING_RAINBOW)
        .setDelay1(100);
    spi0DisplayConnection.send(displayRainbow);
}

  private void scrollText() {
    DisplayMessage textCrawl = new DisplayMessage()
        .setText("Text crawl ...")
        .setDisplayCommand(DisplayCommand.SCROLLING_TEXT)
        .setForegroundGreen(32)
        .setForegroundBlue(32)
        .setDelay1(50)
        .setDelay2(50);
    int bytesSent = spi0DisplayConnection.send(textCrawl);
    System.out.println(
      "Text crawl command sent to display, length = " + bytesSent);
  }

  private void singlePixelNaturalOrder() {
    DisplayMessage naturalOrder = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.SINGLE_PIXEL_NATURAL_ORDER)
        .setForegroundRed(32)
        .setForegroundGreen(32)
        .setDelay1(50);
    spi0DisplayConnection.send(naturalOrder);
  }

  private void singlePixelLeftToRight() {
    DisplayMessage leftToRight = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.SINGLE_PIXEL_LEFT_TO_RIGHT)
        .setForegroundRed(32)
        .setForegroundBlue(32)
        .setDelay1(50);
    spi0DisplayConnection.send(leftToRight);
  }

  private void smoothTextCrawl() {
    DisplayMessage smoothCrawl = new DisplayMessage()
        .setDisplayCommand(DisplayCommand.TEXT_CRAWL)
        .setText("---Smooth---")
        .setForegroundRed(64)
        .setForegroundGreen(32)
        .setForegroundBlue(64)
        .setDelay1(40);
    spi0DisplayConnection.send(smoothCrawl);
  }
}

