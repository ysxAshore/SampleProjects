module top(
  input wire clk,
  input wire rst,
  output wire [7:0] seg0,
  output wire [7:0] seg1,
  output reg led1
);
	reg [24:0] counter;
	reg clk1s;
	always @(posedge clk) begin
		if (rst) begin
			counter <= 'b0;
			clk1s <= 1'b0;
		end
		else if (counter == 24999999)begin
			counter <= 'b0;
			clk1s <= ~clk1s;
		end else begin
			counter <= counter + 1'b1;
		end
	end

	reg [3:0] counter_clk1s[1:0];
	always @(posedge clk1s) begin
		if(counter_clk1s[1] == 4'h9 && counter_clk1s[0] == 4'h9) begin
			counter_clk1s[1] <= 'h0;
			counter_clk1s[0] <= 'h0; 
			led1 <= 1'b1;
		end else if (counter_clk1s[0] == 4'h9) begin
			counter_clk1s[0] <= 'h0;
			counter_clk1s[1] <= counter_clk1s[1] + 1'b1;
		end else begin
			led1 <= 1'b0;
			counter_clk1s[0] <= counter_clk1s[0] + 1'b1;	
		end
	end

	assign seg0 = {8{counter_clk1s[0] == 4'h0}} & 8'b00000011 | 
					{8{counter_clk1s[0] == 4'h1}} & 8'b10011111 |
					{8{counter_clk1s[0] == 4'h2}} & 8'b00100101 |
					{8{counter_clk1s[0] == 4'h3}} & 8'b00001101 |
					{8{counter_clk1s[0] == 4'h4}} & 8'b10011001 |
					{8{counter_clk1s[0] == 4'h5}} & 8'b01001001 |
					{8{counter_clk1s[0] == 4'h6}} & 8'b01000001 |
					{8{counter_clk1s[0] == 4'h7}} & 8'b00011111 |
					{8{counter_clk1s[0] == 4'h8}} & 8'b00000001 |
					{8{counter_clk1s[0] == 4'h9}} & 8'b00001001;

	assign seg1 = {8{counter_clk1s[1] == 4'h0}} & 8'b00000011 | 
					{8{counter_clk1s[1] == 4'h1}} & 8'b10011111 |
					{8{counter_clk1s[1] == 4'h2}} & 8'b00100101 |
					{8{counter_clk1s[1] == 4'h3}} & 8'b00001101 |
					{8{counter_clk1s[1] == 4'h4}} & 8'b10011001 |
					{8{counter_clk1s[1] == 4'h5}} & 8'b01001001 |
					{8{counter_clk1s[1] == 4'h6}} & 8'b01000001 |
					{8{counter_clk1s[1] == 4'h7}} & 8'b00011111 |
					{8{counter_clk1s[1] == 4'h8}} & 8'b00000001 |
					{8{counter_clk1s[1] == 4'h9}} & 8'b00001001;
endmodule
