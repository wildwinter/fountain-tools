import terser from '@rollup/plugin-terser';

console.log(terser);

export default {
  input: "src/fountainTools.js", // Aggregation file as the entry point
  output: [
    {
      file: "dist/fountainTools.js",
      format: "esm", // ES Module format
      sourcemap: true,
    },
    {
      file: "dist/fountainTools.min.js",
      format: "iife", 
      name: "FountainTools", 
      plugins: [terser()],
      sourcemap: true,
    },
  ],
};