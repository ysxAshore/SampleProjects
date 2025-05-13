module top #(
  parameter WIDTH = 8
)(
  input wire [WIDTH-1:0] data_in,
  input wire [$clog2(WIDTH)-1:0] shift_amount,
  input wire direction, // 0 left; 1 right
  input wire shift_type, // 0 logic; 1 arth
  output wire [WIDTH-1:0] data_out
);

wire [WIDTH-1:0] shift_stage[$clog2(WIDTH)-1:0];

assign shift_stage[0] = shift_amount[0] ? 
              (direction ? 
                (shift_type ? {data_in[WIDTH-1],data_in[WIDTH-1:1]} : {1'b0,data_in[WIDTH-1:1]}) :
                  {data_in[WIDTH-2:0],1'b0}):
                    data_in;

generate 
  genvar i;
  for (i = 1; i < $clog2(WIDTH); i = i + 1)begin: SHIFT_LAYERS
    assign shift_stage[i] = shift_amount[i] ? 
                  (direction ? 
                    (shift_type ? {{(2**i){shift_stage[i-1][WIDTH-1]}},shift_stage[i-1][WIDTH-1:2**i]} : {{(2**i){1'b0}},shift_stage[i-1][WIDTH-1:2**i]}) :
                      {shift_stage[i-1][WIDTH-1-2**i:0],{(2**i){1'b0}}}):
                        shift_stage[i-1];
  end
endgenerate

assign data_out = shift_stage[$clog2(WIDTH)-1];

endmodule
