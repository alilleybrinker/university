module control (
          input  wire [5:0] op_code,

          output reg  [3:0] EX,
          output reg  [2:0] M,
          output reg  [1:0] WB
    );

    parameter RTYPE = 6'b000000;
    parameter LW    = 6'b100011;
    parameter SW    = 6'b101011;
    parameter BEQ   = 6'b000100;
    parameter NOOP  = 6'bxxxxxx;

    initial begin
        EX <= 0;
        M  <= 0;
        WB <= 0;
    end

    always@* begin
        case (op_code)
            RTYPE: begin
                EX <= 4'b1100;
                M  <= 3'b000;
                WB <= 2'b10;
            end
            LW: begin
                EX <= 4'b0001;
                M  <= 3'b010;
                WB <= 2'b11;
            end
            SW: begin
                EX <= 4'b0001;
                M  <= 3'b001;
                WB <= 2'b00;
            end
            BEQ: begin
                EX <= 4'b0010;
                M  <= 3'b100;
                WB <= 2'b00;
            end
            NOOP: begin
                EX <= 4'b0000;
                M  <= 3'b000;
                WB <= 2'b00;
            end
            default: $display ("Op code not recognized.");
        endcase
    end
endmodule
