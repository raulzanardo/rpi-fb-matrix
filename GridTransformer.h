// LED matrix library transformer to map a rectangular canvas onto a complex
// chain of matrices.
// Author: Tony DiCola
#ifndef GRIDTRANSFORMER_H
#define GRIDTRANSFORMER_H

#include <cassert>
#include <vector>

#include "led-matrix.h"


class GridTransformer: public rgb_matrix::PixelMapper {
public:
  struct Panel {
    int order;
    int rotate;
    int parallel;
  };

  GridTransformer(int width, int height, int panel_width, int panel_height,
                  int chain_length, const std::vector<Panel>& panels);
  virtual ~GridTransformer() {}

  // PixelMapper interface implementation:
  virtual void MapVisibleToMatrix(int matrix_width, int matrix_height,
                                  int x, int y, int *matrix_x, int *matrix_y) const;

  // Other attribute accessors.
  int getRows() const {
    return _rows;
  }
  int getColumns() const {
    return _cols;
  }
  int getWidth() const {
    return _width;
  }
  int getHeight() const {
    return _height;
  }

private:
  int _width,
      _height,
      _panel_width,
      _panel_height,
      _chain_length,
      _rows,
      _cols;
  std::vector<Panel> _panels;
};

#endif
