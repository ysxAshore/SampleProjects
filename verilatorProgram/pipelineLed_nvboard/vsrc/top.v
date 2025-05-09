module top(
  input wire clk,
  input wire rst,
  output reg [15:0] ledr
);
	reg [20:0] counter;
	always @(posedge clk)begin
		if(rst)begin
			ledr <= 16'b1;
			counter <= 21'b0;
		end else begin
			counter <= counter + 21'b1;
			if(counter == 21'b0)begin
				ledr <= {ledr[14:0],ledr[15]};
			end
		end
	end
endmodule
