package org.metuchenmomentum.marquee;

/**
 * Specifies a color via red, green, and blue intensities.
 */
public record RGBColor(
  /**
   * Red intensity, 0 - 255. Only the low byte is significant.
   * The high byte is ignored
   */
  int red,
  /**
   * Green intensity, 0 -- 255. Only the low byte is significant.
   * The high byte is ignored.
   */
  int green,
  /**
   * Blue intensity, 0 -- 255. Only the low byte is significant.
   * The high byte is ignored.
   */
  int blue) {
}
