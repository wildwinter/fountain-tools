import terser from '@rollup/plugin-terser';

console.log(terser);

export default {
  input: "src/screenplayTools.js", // Aggregation file as the entry point
  output: [
    {
      file: "dist/screenplayTools.js",
      format: "esm", // ES Module format
      sourcemap: true,
    },
    {
      file: "dist/screenplayTools.min.js",
      format: "iife",
      name: "ScreenplayTools",
      plugins: [terser()],
      sourcemap: true,
    },
  ],
};